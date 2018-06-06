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
	"strings"

	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/memio"
	"github.com/regomne/eutil/textFile"
)

type keyOps struct {
	msgOp  uint8
	callOp uint8
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

func decryptYbn(stm []byte, key []byte) {
	if len(key) != 4 {
		panic("key length error")
	}
	for i := 0x20; i < len(stm); i++ {
		stm[i] ^= key[i&3]
	}
}

func parseYbn(stm io.ReadSeeker) (script ybnInfo, err error) {
	binary.Read(stm, binary.LittleEndian, &script.Header)
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
	stm.Seek(int64(binary.Size(header)), 0)
	script.Insts = make([]instInfo, header.InstCnt)
	rawInsts := make([]YInst, header.InstCnt)
	binary.Read(stm, binary.LittleEndian, &rawInsts)
	var tArg YArg
	rargs := make([]YArg, header.ArgSize/uint32(binary.Size(tArg)))
	binary.Read(stm, binary.LittleEndian, &rargs)
	resStartOff, _ := stm.Seek(0, 1)
	rargIdx := 0
	//lastOp := uint8(0)
	//lastResOff := uint32(0)
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
				stm.Seek(resStartOff+int64(rarg.ResOffset), 0)
				res := &inst.Args[j].Res
				if rarg.Type == 3 {
					var resInfo YResInfo
					binary.Read(stm, binary.LittleEndian, &resInfo)
					res.Type = resInfo.Type
					res.Res = make([]byte, resInfo.Len)
					stm.Read(res.Res)
				} else {
					res.ResRaw = make([]byte, rarg.ResSize)
					stm.Read(res.ResRaw)
				}
			}
		}
		//lastOp = inst.Op
	}
	offTblOffset := uint32(binary.Size(header)) + header.CodeSize + header.ArgSize + header.ResourceSize
	stm.Seek(int64(offTblOffset), 0)
	script.Offs = make([]uint32, header.InstCnt)
	binary.Read(stm, binary.LittleEndian, &script.Offs)
	return
}

func guessYbnOp(script *ybnInfo, ops *keyOps) bool {
	if ops.callOp != 0 && ops.msgOp != 0 {
		return true
	}
	msgStat := [256]int{0}
	callStat := [256]int{0}
	for _, inst := range script.Insts {
		if ops.msgOp == 0 && len(inst.Args) == 1 &&
			inst.Args[0].Type == 0 && inst.Args[0].Value == 0 {
			res := &inst.Args[0].Res
			if len(res.ResRaw) != 0 && res.ResRaw[0] > 0x80 {
				msgStat[inst.Op]++
				if msgStat[inst.Op] > 10 {
					ops.msgOp = inst.Op
				}
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
		if ops.msgOp != 0 && ops.callOp != 0 {
			return true
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
			txt = append(txt, codec.Decode(inst.Args[0].Res.ResRaw, codePage))
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
		}
	}
	return
}

func parseYbnFile(ybnName, outScriptName, outTxtName string, key []byte, ops *keyOps, codePage int) bool {
	oriStm, err := ioutil.ReadFile(ybnName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	if bytes.Compare(key, []byte("\x00\x00\x00\x00")) != 0 {
		decryptYbn(oriStm, key)
	}
	reader := bytes.NewReader(oriStm)
	script, err := parseYbn(reader)
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	if !guessYbnOp(&script, ops) {
		fmt.Println("Can't guess the opcode")
		return false
	}
	if outScriptName != "" {
		decodeScriptString(&script, ops, codePage)
		out, err := json.MarshalIndent(script, "", "\t")
		if err != nil {
			fmt.Println("error when marshalling json:", err)
			return false
		}
		ioutil.WriteFile(outScriptName, out, os.ModePerm)
	}
	if outTxtName != "" {
		txt, err := extTxtFromYbn(&script, ops, codePage)
		if err != nil {
			fmt.Println("error when extracting txt:", err)
			return false
		}
		out := codec.Encode(strings.Join(txt, "\r\n"), codec.UTF8Sig, codec.Replace)
		ioutil.WriteFile(outTxtName, out, os.ModePerm)
	}
	return true
}

func packTxtToYbn(script *ybnInfo, stm []byte, txt []string, ops *keyOps, codePage int) (newStm []byte, err error) {
	argOffStart := uint32(binary.Size(script.Header)) + script.Header.CodeSize
	argStm := memio.NewWithBytes(stm[argOffStart : argOffStart+script.Header.ArgSize])

	var resTail bytes.Buffer

	resNewOffset := script.Header.ResourceSize
	argIdx := 0
	txtIdx := 0
	for _, inst := range script.Insts {
		if inst.Op == ops.msgOp {
			ns := codec.Encode(txt[txtIdx], codePage, codec.Replace)
			txtIdx++
			resTail.Write(ns)
			argStm.Seek(int64(argIdx*12)+4, 0)
			binary.Write(argStm, binary.LittleEndian, len(ns))
			binary.Write(argStm, binary.LittleEndian, resNewOffset)
			resNewOffset += uint32(len(ns))
		} else if inst.Op == ops.callOp {
			if isFunctionToExtract(inst.Args[0].Res.Res) {
				for i, arg := range inst.Args[1:] {
					if arg.Type == 3 &&
						bytes.Compare(arg.Res.Res, []byte(`""`)) != 0 &&
						bytes.Compare(arg.Res.Res, []byte(`''`)) != 0 {
						ns := codec.Encode(txt[txtIdx], codePage, codec.Replace)
						txtIdx++
						var resInfo YResInfo
						resInfo.Type = arg.Res.Type
						resInfo.Len = uint16(len(ns))
						binary.Write(&resTail, binary.LittleEndian, &resInfo)
						resTail.Write(ns)
						argStm.Seek(int64((argIdx+1+i)*12)+4, 0)
						binary.Write(argStm, binary.LittleEndian, len(ns)+binary.Size(resInfo))
						binary.Write(argStm, binary.LittleEndian, resNewOffset)
						resNewOffset += uint32(len(ns) + binary.Size(resInfo))
					}
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
	return newYbn.Bytes(), nil
}

func packYbnFile(ybnName, txtName, outYbnName string, key []byte, ops *keyOps, codePage int) bool {
	oriStm, err := ioutil.ReadFile(ybnName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	if bytes.Compare(key, []byte("\x00\x00\x00\x00")) != 0 {
		decryptYbn(oriStm, key)
	}
	reader := bytes.NewReader(oriStm)
	script, err := parseYbn(reader)
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	if !guessYbnOp(&script, ops) {
		fmt.Println("Can't guess the opcode")
		return false
	}
	ls, err := textFile.ReadWin32TxtToLines(txtName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	newStm, err := packTxtToYbn(&script, oriStm, ls, ops, codePage)
	if err != nil {
		fmt.Println(err)
		return false
	}
	if bytes.Compare(key, []byte("\x00\x00\x00\x00")) != 0 {
		decryptYbn(newStm, key)
	}
	ioutil.WriteFile(outYbnName, newStm, os.ModePerm)
	return true
}

func printUsage(exeName string) {
	fmt.Printf("Usage: %s -e -ybn <ybn> [-json <json>] [-txt <txt>] [options]\n", exeName)
	fmt.Printf("Usage: %s -p -ybn <ybn> -txt <txt> -new-ybn <new_ybn> [options]\n", exeName)
	flag.Usage()
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

func main() {
	retCode := 0
	defer os.Exit(retCode)
	isExtract := flag.Bool("e", false, "extract a ybn")
	isPack := flag.Bool("p", false, "pack a ybn")
	inYbnName := flag.String("ybn", "", "input ybn file name")
	jsonName := flag.String("json", "", "output script file name")
	txtName := flag.String("txt", "", "output/input txt file name")
	outYbnName := flag.String("new-ybn", "", "output ybn file name")
	keyInt := flag.Int("key", 0x96ac6fd3, "decode key of ybn")
	opMsg := flag.Int("op-msg", 0, "specify opcode of Msg. defaut: auto guess")
	opCall := flag.Int("op-call", 0, "specify opcode of Call. default: auto guess")
	codePage := flag.String("cp", "932", "specify code page")
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

	ops := keyOps{uint8(*opMsg), uint8(*opCall)}
	//ops := keyOps{108, 43}

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
