package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"strconv"
	"strings"

	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/memio"
	"github.com/regomne/eutil/textFile"
)

type keyOps struct {
	msgOp   uint8
	callOp  uint8
	otherOp []uint8
}

type resourceEntry struct {
	Type   uint8
	Res    []byte `json:",omitempty"`
	ResRaw []byte `json:",omitempty"`
	ResStr string `json:",omitempty"`
}

type argInfo struct {
	Value uint16
	Type  uint16
	Res   resourceEntry

	ResInfo   uint32 `json:",omitempty"`
	ResOffset uint32 `json:",omitempty"`
}

type instInfo struct {
	Op   uint8
	Unk  uint16
	Args []argInfo
}

type ybnInfo struct {
	Header YbnHeader
	Insts  []instInfo
	Offs   []uint32
}

var gIsOutputOpcode bool
var gVerbose bool

func logf(fmts string, args ...interface{}) {
	if gVerbose {
		fmt.Printf(fmts, args...)
	}
}

func logln(args ...interface{}) {
	if gVerbose {
		fmt.Println(args...)
	}
}

func includes(a []uint8, v uint8) bool {
	for _, i := range a {
		if i == v {
			return true
		}
	}
	return false
}

func decryptBlock(stm []byte, key []byte) {
	if len(key) != 4 {
		panic("key length error")
	}
	for i := 0; i < len(stm); i++ {
		stm[i] ^= key[i&3]
	}
}

func decryptYbn(stm []byte, key []byte, header *YbnHeader) {
	p := uint32(binary.Size(*header))
	decryptBlock(stm[p:p+header.CodeSize], key)
	p += header.CodeSize
	decryptBlock(stm[p:p+header.ArgSize], key)
	p += header.ArgSize
	decryptBlock(stm[p:p+header.ResourceSize], key)
	p += header.ResourceSize
	decryptBlock(stm[p:p+header.OffSize], key)
}

func parseYbn(oriStm []byte, key []byte) (script ybnInfo, err error) {
	stm := bytes.NewReader(oriStm)
	binary.Read(stm, binary.LittleEndian, &script.Header)
	logln("header:", script.Header)
	header := &script.Header
	if bytes.Compare(header.Magic[:], []byte("YSTB")) != 0 ||
		header.CodeSize != header.InstCnt*4 {
		err = fmt.Errorf("not a ybn file or file format error")
		return
	}
	fileSize, _ := stm.Seek(0, 2)
	if uint32(binary.Size(header))+header.CodeSize+header.ArgSize+header.ResourceSize+
		header.OffSize != uint32(fileSize) {
		err = fmt.Errorf("file size error")
		return
	}
	if header.Resv != 0 {
		fmt.Println("reserved is not 0, maybe can't extract all the info")
	}
	var decryptedStm io.ReadSeeker
	if bytes.Compare(key, []byte("\x00\x00\x00\x00")) != 0 {
		logf("decrypting, key is:%02x %02x %02x %02x\n", key[0], key[1], key[2], key[3])
		decryptYbn(oriStm, key, header)
		decryptedStm = bytes.NewReader(oriStm)
	} else {
		decryptedStm = bytes.NewReader(oriStm)
	}

	//logln("write decrypted file...")
	//s := []string{*inYbnName, ".decrypt"}
	//err1 := ioutil.WriteFile(strings.Join(s, ""), oriStm, 0644)
	//fmt.Println(err1)

	logln("reading sections...")
	decryptedStm.Seek(int64(binary.Size(header)), 0)
	script.Insts = make([]instInfo, header.InstCnt)
	rawInsts := make([]YInst, header.InstCnt)
	binary.Read(decryptedStm, binary.LittleEndian, &rawInsts)
	var tArg YArg
	rargs := make([]YArg, header.ArgSize/uint32(binary.Size(tArg)))
	binary.Read(decryptedStm, binary.LittleEndian, &rargs)
	resStartOff, _ := decryptedStm.Seek(0, 1)
	rargIdx := 0
	logln("parsing instructions...")
	for i, rinst := range rawInsts {
		inst := &script.Insts[i]
		inst.Op = rinst.Op
		inst.Unk = rinst.Unk
		inst.Args = make([]argInfo, rinst.ArgCnt)
		for j := 0; j < int(rinst.ArgCnt); j++ {
			rarg := &rargs[rargIdx]
			rargIdx++
			if rargIdx > len(rargs) {
				err = fmt.Errorf("count of arguments exceed limit")
				return
			}
			inst.Args[j].Type = rarg.Type
			inst.Args[j].Value = rarg.Value
			if rarg.Type == 0 && rinst.ArgCnt != 1 {
				inst.Args[j].ResInfo = rarg.ResSize
				inst.Args[j].ResOffset = rarg.ResOffset
			} else {
				decryptedStm.Seek(resStartOff+int64(rarg.ResOffset), 0)
				res := &inst.Args[j].Res
				if rarg.Type == 3 {
					var resInfo YResInfo
					binary.Read(decryptedStm, binary.LittleEndian, &resInfo)
					res.Type = resInfo.Type
					res.Res = make([]byte, resInfo.Len)
					decryptedStm.Read(res.Res)
				} else {
					res.ResRaw = make([]byte, rarg.ResSize)
					decryptedStm.Read(res.ResRaw)
				}
			}
		}
	}
	offTblOffset := uint32(binary.Size(header)) + header.CodeSize + header.ArgSize + header.ResourceSize
	decryptedStm.Seek(int64(offTblOffset), 0)
	script.Offs = make([]uint32, header.InstCnt)
	binary.Read(decryptedStm, binary.LittleEndian, &script.Offs)
	return
}

func isLongEnglishSentence(s []byte) bool {
	spaceCount := 0
	for _, c := range s {
		if c >= 0x80 {
			return false
		} else if c == ' ' {
			spaceCount++
		}
	}
	if spaceCount > 5 {
		return true
	}
	return false
}

func isEnglishMsg(arg argInfo) bool {
	if arg.Value == 0 && arg.Type == 3 {
		return isLongEnglishSentence(arg.Res.Res)
	}
	return false
}

func isJapOrChnMsg(arg argInfo) bool {
	if arg.Value == 0 && arg.Type == 0 {
		if len(arg.Res.ResRaw) != 0 && arg.Res.ResRaw[0] > 0x80 {
			return true
		}
	}
	return false
}

func guessYbnOp(script *ybnInfo, ops *keyOps) bool {
	msgStat := [256]int{0}
	callStat := [256]int{0}
	for _, inst := range script.Insts {
		if ops.msgOp != 0 && ops.callOp != 0 {
			if gIsOutputOpcode {
				fmt.Println("msg\tcall")
				fmt.Printf("%d\t%d\n", ops.msgOp, ops.callOp)
			}
			return true
		}
		if ops.msgOp == 0 && len(inst.Args) == 1 &&
			(isJapOrChnMsg(inst.Args[0]) || isEnglishMsg(inst.Args[0])) {
			msgStat[inst.Op]++
			if msgStat[inst.Op] > 10 {
				ops.msgOp = inst.Op
			}
		}
		if ops.callOp == 0 && len(inst.Args) >= 1 &&
			inst.Args[0].Value == 0 && inst.Args[0].Type == 3 {
			res := &inst.Args[0].Res
			s := string(res.Res)
			if res.Type == 0x4d && len(s) > 4 &&
				s[0] == '"' && s[1] == 'e' && s[len(s)-1] == '"' {
				callStat[inst.Op]++
				if callStat[inst.Op] > 5 {
					ops.callOp = inst.Op
				}
			}
		}
	}
	return false
}

func decodeScriptString(script *ybnInfo, ops *keyOps, codePage int) {
	for i := range script.Insts {
		inst := &script.Insts[i]
		for j := range inst.Args {
			res := &inst.Args[j].Res
			if len(res.Res) != 0 {
				res.ResStr = codec.Decode(res.Res, codePage)
			} else if inst.Op == ops.msgOp {
				res.ResStr = codec.Decode(res.ResRaw, codePage)
			}
		}
	}
}

func isFunctionToExtract(name []byte) bool {
	names := GetTextFunctionNames()
	f := strings.ToLower(string(name))
	for _, v := range names {
		if strings.Compare(v, f) == 0 {
			return true
		}
	}
	return false
}

func extTxtFromYbn(script *ybnInfo, ops *keyOps, codePage int) (txt []string, err error) {
	txt = make([]string, 0, len(script.Insts)/3)
	for _, inst := range script.Insts {
		if inst.Op == ops.msgOp {
			if len(inst.Args) != 1 {
				err = fmt.Errorf("the message op:0x%x has not only 1 argument", ops.msgOp)
				return
			}
			rawStr := []byte{}
			if inst.Args[0].Type == 3 {
				// for English games, it seems the msg op uses type-3 resource
				rawStr = inst.Args[0].Res.Res
			} else {
				// and for Japnese games, it usually uses raw resource
				rawStr = inst.Args[0].Res.ResRaw
			}
			txt = append(txt, codec.Decode(rawStr, codePage))
		} else if inst.Op == ops.callOp {
			if len(inst.Args) < 1 {
				err = fmt.Errorf("call op:0x%x argument less than 1", ops.callOp)
				return
			}
			if isFunctionToExtract(inst.Args[0].Res.Res) {
				for _, arg := range inst.Args[1:] {
					if arg.Type == 3 &&
						bytes.Compare(arg.Res.Res, []byte(`""`)) != 0 &&
						bytes.Compare(arg.Res.Res, []byte(`''`)) != 0 {
						txt = append(txt, codec.Decode(arg.Res.Res, codePage))
					}
				}
			}
		} else if includes(ops.otherOp, inst.Op) {
			for _, arg := range inst.Args {
				if arg.Type == 3 &&
					bytes.Compare(arg.Res.Res, []byte(`""`)) != 0 &&
					bytes.Compare(arg.Res.Res, []byte(`''`)) != 0 {
					txt = append(txt, codec.Decode(arg.Res.Res, codePage))
				}
			}

		}
	}
	return
}

func parseYbnFile(ybnName, outScriptName, outTxtName string, key []byte, ops *keyOps, codePage int) bool {
	logln("reading file:", ybnName)
	oriStm, err := ioutil.ReadFile(ybnName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	logln("parsing ybn...")
	script, err := parseYbn(oriStm, key)
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	logln("guessing opcode if not provided...")
	if !guessYbnOp(&script, ops) {
		fmt.Printf("Guess opcodes failed, msg op:0x%x, call op:0x%x", ops.msgOp, ops.callOp)
		//return false
	}
	if outScriptName != "" {
		logln("writing json...")
		if gVerbose {
			logln("decode some string in json...")
			decodeScriptString(&script, ops, codePage)
		}
		out, err := json.MarshalIndent(script, "", "\t")
		if err != nil {
			fmt.Println("error when marshalling json:", err)
			return false
		}
		ioutil.WriteFile(outScriptName, out, os.ModePerm)
	}
	if outTxtName != "" {
		logln("extracting text from script...")
		txt, err := extTxtFromYbn(&script, ops, codePage)
		if err != nil {
			fmt.Println("error when extracting txt:", err)
			return false
		}
		logln("encoding text and writing...")
		out := codec.Encode(strings.Join(txt, "\r\n"), codec.UTF8Sig, codec.Replace)
		ioutil.WriteFile(outTxtName, out, os.ModePerm)
	}
	logln("complete.")
	return true
}

func packLineToResource(arg argInfo, line string, cp int) []byte {
	ns := codec.Encode(line, cp, codec.ReplaceHTML)
	if arg.Type == 3 {
		var bf bytes.Buffer
		var resInfo YResInfo
		resInfo.Type = arg.Res.Type
		resInfo.Len = uint16(len(ns))
		binary.Write(&bf, binary.LittleEndian, &resInfo)
		bf.Write(ns)
		return bf.Bytes()
	}
	return ns
}

func packTxtToYbn(script *ybnInfo, stm []byte, txt []string, ops *keyOps, codePage int, key []byte) (newStm []byte, err error) {
	argOffStart := uint32(binary.Size(script.Header)) + script.Header.CodeSize
	argStm := memio.NewWithBytes(stm[argOffStart : argOffStart+script.Header.ArgSize])

	var resTail bytes.Buffer

	resNewOffset := script.Header.ResourceSize
	argIdx := 0
	txtIdx := 0
	for _, inst := range script.Insts {
		if inst.Op == ops.msgOp {
			ns := packLineToResource(inst.Args[0], txt[txtIdx], codePage)
			txtIdx++
			resTail.Write(ns)
			argStm.Seek(int64(argIdx*12)+4, 0)
			binary.Write(argStm, binary.LittleEndian, uint32(len(ns)))
			binary.Write(argStm, binary.LittleEndian, resNewOffset)
			resNewOffset += uint32(len(ns))
		} else if inst.Op == ops.callOp {
			if isFunctionToExtract(inst.Args[0].Res.Res) {
				for i, arg := range inst.Args[1:] {
					if arg.Type == 3 &&
						bytes.Compare(arg.Res.Res, []byte(`""`)) != 0 &&
						bytes.Compare(arg.Res.Res, []byte(`''`)) != 0 {
						ns := packLineToResource(arg, txt[txtIdx], codePage)
						txtIdx++
						resTail.Write(ns)
						argStm.Seek(int64((argIdx+1+i)*12)+4, 0)
						binary.Write(argStm, binary.LittleEndian, uint32(len(ns)))
						binary.Write(argStm, binary.LittleEndian, resNewOffset)
						resNewOffset += uint32(len(ns))
					}
				}
			}
		} else if includes(ops.otherOp, inst.Op) {
			for i, arg := range inst.Args {
				if arg.Type == 3 &&
					bytes.Compare(arg.Res.Res, []byte(`""`)) != 0 &&
					bytes.Compare(arg.Res.Res, []byte(`''`)) != 0 {
					ns := packLineToResource(arg, txt[txtIdx], codePage)
					txtIdx++
					resTail.Write(ns)
					argStm.Seek(int64((argIdx+i)*12)+4, 0)
					binary.Write(argStm, binary.LittleEndian, uint32(len(ns)))
					binary.Write(argStm, binary.LittleEndian, resNewOffset)
					resNewOffset += uint32(len(ns))
				}
			}
		}
		argIdx += len(inst.Args)
	}

	var newYbn bytes.Buffer
	newHdr := script.Header
	newHdr.ResourceSize += uint32(resTail.Len())
	binary.Write(&newYbn, binary.LittleEndian, &newHdr)
	codeStart := uint32(binary.Size(newHdr))
	newYbn.Write(stm[codeStart : codeStart+script.Header.CodeSize])
	newYbn.Write(argStm.Bytes())
	resStart := argOffStart + script.Header.ArgSize
	newYbn.Write(stm[resStart : resStart+script.Header.ResourceSize])
	newYbn.Write(resTail.Bytes())
	offStart := resStart + script.Header.ResourceSize
	newYbn.Write(stm[offStart : offStart+script.Header.OffSize])

	outputStm := newYbn.Bytes()
	if bytes.Compare(key, []byte("\x00\x00\x00\x00")) != 0 {
		decryptYbn(outputStm, key, &newHdr)
	}

	return outputStm, nil
}

func packYbnFile(ybnName, txtName, outYbnName string, key []byte, ops *keyOps, codePage int) bool {
	logln("reading file:", ybnName)
	oriStm, err := ioutil.ReadFile(ybnName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	logln("parsing ybn...")
	script, err := parseYbn(oriStm, key)
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	logln("guessing opcode if not provided...")
	if !guessYbnOp(&script, ops) {
		fmt.Println("Can't guess the opcode")
		return false
	}
	logln("reading text:", txtName)
	ls, err := textFile.ReadWin32TxtToLines(txtName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	logf("reading text finished, %d lines\n", len(ls))
	logln("packing text to ybn...")
	newStm, err := packTxtToYbn(&script, oriStm, ls, ops, codePage, key)
	if err != nil {
		fmt.Println(err)
		return false
	}
	logln("writing ybn:", outYbnName)
	ioutil.WriteFile(outYbnName, newStm, os.ModePerm)
	logln("complete.")
	return true
}

func printUsage(exeName string) {
	fmt.Println("YBN extractor v3.0")
	fmt.Printf("Usage: %s -e -ybn <ybn> [-json <json>] [-txt <txt>] [options]\n", exeName)
	fmt.Printf("Usage: %s -p -ybn <ybn> -txt <txt> -new-ybn <new_ybn> [options]\n", exeName)
	flag.Usage()

	fmt.Printf(`
About the key:
  I don't know any way to guess the key except analyzing the bin. Most games
  use the default key, but if not, I have no idea about it.
  The program may CRASH OR PANIC if not given a correct key.
  You may try: 0x6cfddadb or 0x30731B78.

About the opcode:
  This program can guess the opcode-msg and opcode-call which is needed, but
  you need to give it a .ybn which has some msg texts to do it. Generally, you
  can give it yst0XXXX.ybn where the XXXX is the maximum number among all the
  ybn file names.
  You can use:

  extYbn -e -ybn yst0XXXX.ybn -output-opcode

  to output the opcodes, and then use them in all the .ybn files of this game.
  You can also use the -op-other to use other ops to extract or pack text.
`)
}

func parseCp(s string) int {
	switch s {
	case "936":
		return codec.C936
	case "932":
		return codec.C932
	default:
		return codec.Unknown
	}
}

func parseCmdlineOp(ops string) (oplist []uint8, suc bool) {
	oplist = make([]uint8, 0)
	suc = true
	if ops == "" {
		return
	}
	for _, o := range strings.Split(ops, ",") {
		op64, err := strconv.ParseUint(o, 0, 8)
		if err != nil {
			fmt.Println("op text op error:", err)
			suc = false
			return
		}
		oplist = append(oplist, uint8(op64))
	}
	return
}

func main() {
	retCode := 0
	defer os.Exit(retCode)
	isExtract := flag.Bool("e", false, "extract a ybn")
	isPack := flag.Bool("p", false, "pack a ybn")
	inYbnName := flag.String("ybn", "", "input ybn file name")
	jsonName := flag.String("json", "", "output script file name")
	txtName := flag.String("txt", "", "output/input txt file name")
	outYbnName := flag.String("new-ybn", "", "output ybn file name")
	keyInt := flag.Int64("key", 0x96ac6fd3, "decode key of ybn")
	opMsg := flag.Int("op-msg", 0, "specify opcode of Msg. defaut: auto guess")
	opCall := flag.Int("op-call", 0, "specify opcode of Call. default: auto guess")
	codePage := flag.String("cp", "932", "specify code page")
	outputOpCode := flag.Bool("output-opcode", false, "output the opcode guessed")
	otherTextOp := flag.String("op-other", "", "other op to extract/pack txt(comma to split, i.e: 3,4,100")
	verbose := flag.Bool("v", false, "verbose output, and decode some string when outputs in json")
	flag.Parse()
	if (!*isExtract && !*isPack) || (*isExtract && *isPack) ||
		*inYbnName == "" ||
		(*isPack && (*outYbnName == "" || *txtName == "")) {
		printUsage(os.Args[0])
		return
	}

	key := [4]byte{}
	key[0] = byte(*keyInt & 0xff)
	key[1] = byte((*keyInt >> 8) & 0xff)
	key[2] = byte((*keyInt >> 16) & 0xff)
	key[3] = byte((*keyInt >> 24) & 0xff)

	ops := keyOps{uint8(*opMsg), uint8(*opCall), nil}
	textOps, suc := parseCmdlineOp(*otherTextOp)
	if !suc {
		retCode = 1
		return
	}
	ops.otherOp = textOps

	gIsOutputOpcode = *outputOpCode
	gVerbose = *verbose

	var ret bool
	if *isExtract {
		ret = parseYbnFile(*inYbnName, *jsonName, *txtName, key[:], &ops, parseCp(*codePage))
	} else {
		ret = packYbnFile(*inYbnName, *txtName, *outYbnName, key[:], &ops, parseCp(*codePage))
	}
	if !ret {
		retCode = 1
	}
}
