package main

import (
	"bytes"
	"encoding/base64"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/memio"
	"github.com/regomne/eutil/textFile"
	"io"
	"os"
	"strconv"
	"strings"
)

type ystbResourceEntry struct {
	Type   uint8
	Res    []byte `json:",omitempty"`
	ResRaw []byte `json:",omitempty"`
	ResStr string `json:",omitempty"`
}

type ystbArgInfo struct {
	Value uint16
	Type  uint16
	Res   ystbResourceEntry

	ResInfo   uint32 `json:",omitempty"`
	ResOffset uint32 `json:",omitempty"`
}

type ystbInstInfo struct {
	Op      uint8
	LabelId uint16
	Args    []ystbArgInfo
}

type ystbInfo struct {
	Header ystbHeader
	Insts  []ystbInstInfo
	Offs   []uint32
}

type ystbHeader struct {
	Meta         YbnHeader
	InstCnt      uint32
	CodeSize     uint32
	ArgSize      uint32
	ResourceSize uint32
	OffSize      uint32
	Resv         uint32
}

type ystbInst struct {
	Op      uint8
	ArgCnt  uint8
	LabelId uint16
}

type ystbArg struct {
	Value     uint16
	Type      uint16
	ResSize   uint32
	ResOffset uint32
}

type ystbResInfo struct {
	Type uint8
	Len  uint16
}

func GetTextFunctionNames() []string {
	return []string{
		`"es.sel.set"`,
		`"es.char.name.mark.set"`,
		`"es.char.name"`,
		`"es.input.str.set"`,
		`"es.tips.def.set"`,
		`"es.tips.tx.def.set"`,
	}
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

func isEnglishMsg(arg ystbArgInfo) bool {
	if arg.Value == 0 && arg.Type == 3 {
		return isLongEnglishSentence(arg.Res.Res)
	}
	return false
}

func isJapOrChnMsg(arg ystbArgInfo) bool {
	if arg.Value == 0 && arg.Type == 0 {
		if len(arg.Res.ResRaw) != 0 && arg.Res.ResRaw[0] > 0x80 {
			return true
		}
	}
	return false
}

func guessYstbOp(script *ystbInfo, ops *[256]string) bool {
	msgStat := [256]int{0}
	callStat := [256]int{0}
	for _, inst := range script.Insts {
		if includes(ops[:], "msg") && includes(ops[:], "call") {
			if gIsOutputOpcode {
				fmt.Println("msg\tcall")
				fmt.Printf("%d\t%d\n", IndexOf(ops[:], "msg"), IndexOf(ops[:], "call"))
			}
			return true
		}
		if !includes(ops[:], "msg") && len(inst.Args) == 1 && (isJapOrChnMsg(inst.Args[0]) || isEnglishMsg(inst.Args[0])) {
			msgStat[inst.Op]++
			if msgStat[inst.Op] > 10 {
				ops[inst.Op] = "msg"
			}
		}
		if !includes(ops[:], "call") && len(inst.Args) >= 1 &&
			inst.Args[0].Value == 0 && inst.Args[0].Type == 3 {
			res := &inst.Args[0].Res
			s := string(res.Res)
			if res.Type == 0x4d && len(s) > 4 &&
				s[0] == '"' && s[1] == 'e' && s[len(s)-1] == '"' {
				callStat[inst.Op]++
				if callStat[inst.Op] > 5 {
					ops[inst.Op] = "call"
				}
			}
		}
	}
	return false
}

func decryptYstb(stm []byte, key []byte, header *ystbHeader) {
	p := uint32(binary.Size(*header))
	decryptBlock(stm[p:p+header.CodeSize], key)
	p += header.CodeSize
	decryptBlock(stm[p:p+header.ArgSize], key)
	p += header.ArgSize
	decryptBlock(stm[p:p+header.ResourceSize], key)
	p += header.ResourceSize
	decryptBlock(stm[p:p+header.OffSize], key)
}

func packLineToYstbResource(arg ystbArgInfo, line string, cp int) []byte {
	ns := codec.Encode(line, cp, codec.ReplaceHTML)
	if arg.Type == 3 {
		var bf bytes.Buffer
		var resInfo ystbResInfo
		resInfo.Type = arg.Res.Type
		resInfo.Len = uint16(len(ns))
		binary.Write(&bf, binary.LittleEndian, &resInfo)
		bf.Write(ns)
		return bf.Bytes()
	}
	return ns
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

func packTxtToYstb(script *ystbInfo, stm []byte, txt []string, ops *[256]string, codePage int, key []byte) (newStm []byte, err error) {
	argOffStart := uint32(binary.Size(script.Header)) + script.Header.CodeSize
	argStm := memio.NewWithBytes(stm[argOffStart : argOffStart+script.Header.ArgSize])

	var resTail bytes.Buffer

	resNewOffset := script.Header.ResourceSize
	argIdx := 0
	txtIdx := 0
	for _, inst := range script.Insts {
		if ops[inst.Op] == "msg" {
			ns := packLineToYstbResource(inst.Args[0], txt[txtIdx], codePage)
			txtIdx++
			resTail.Write(ns)
			argStm.Seek(int64(argIdx*12)+4, 0)
			binary.Write(argStm, binary.LittleEndian, uint32(len(ns)))
			binary.Write(argStm, binary.LittleEndian, resNewOffset)
			resNewOffset += uint32(len(ns))
		} else if ops[inst.Op] == "call" {
			if isFunctionToExtract(inst.Args[0].Res.Res) {
				for i, arg := range inst.Args[1:] {
					if arg.Type == 3 &&
						bytes.Compare(arg.Res.Res, []byte(`""`)) != 0 &&
						bytes.Compare(arg.Res.Res, []byte(`''`)) != 0 {
						ns := packLineToYstbResource(arg, txt[txtIdx], codePage)
						txtIdx++
						resTail.Write(ns)
						argStm.Seek(int64((argIdx+1+i)*12)+4, 0)
						binary.Write(argStm, binary.LittleEndian, uint32(len(ns)))
						binary.Write(argStm, binary.LittleEndian, resNewOffset)
						resNewOffset += uint32(len(ns))
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

	outputStm := newYbn.Bytes()
	if bytes.Compare(key, []byte("\x00\x00\x00\x00")) != 0 {
		decryptYstb(outputStm, key, &newHdr)
	}

	return outputStm, nil
}

func packYstbFile(oriStm []byte, txtName, outYbnName string, key []byte, ops *[256]string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYstb(oriStm, key, "")
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	logln("guessing opcode if not provided...")
	if !guessYstbOp(&script, ops) {
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
	newStm, err := packTxtToYstb(&script, oriStm, ls, ops, codePage, key)
	if err != nil {
		fmt.Println(err)
		return false
	}
	logln("writing ybn:", outYbnName)
	os.WriteFile(outYbnName, newStm, os.ModePerm)
	logln("complete.")
	return true
}

func decodeScriptString(script *ystbInfo, ops *[256]string, codePage int) {
	for i := range script.Insts {
		inst := &script.Insts[i]
		for j := range inst.Args {
			res := &inst.Args[j].Res
			if len(res.Res) != 0 && res.Type == 77 {
				res.ResStr = codec.Decode(res.Res, codePage)
			} else if ops[inst.Op] == "msg" {
				res.ResStr = codec.Decode(res.ResRaw, codePage)
			}
		}
	}
}

func extTxtFromYbn(script *ystbInfo, ops *[256]string, codePage int) (txt []string, err error) {
	txt = make([]string, 0, len(script.Insts)/3)
	for _, inst := range script.Insts {
		if ops[inst.Op] == "msg" {
			if len(inst.Args) != 1 {
				err = fmt.Errorf("the message op:0x%x has not only 1 argument", inst.Op)
				return
			}
			rawStr := []byte{}
			if inst.Args[0].Type == 3 {
				// for English games, it seems the msg op uses type-3 resource
				rawStr = inst.Args[0].Res.Res
			} else {
				// and for Japanese games, it usually uses raw resource
				rawStr = inst.Args[0].Res.ResRaw
			}
			txt = append(txt, codec.Decode(rawStr, codePage))
		} else if ops[inst.Op] == "call" {
			if len(inst.Args) < 1 {
				err = fmt.Errorf("call op:0x%x argument less than 1", inst.Op)
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

func resStr(res ystbResourceEntry, codePage int) string {
	if len(res.ResStr) != 0 {
		return res.ResStr
	} else if len(res.Res) != 0 {
		return codec.Decode(res.Res, codePage)
	} else if len(res.ResRaw) != 0 {
		return codec.Decode(res.ResRaw, codePage)
	}
	return ""
}

func parseYstbFile(oriStm []byte, outJsonName, outTxtName, outDecryptName, outInstructName string, key []byte, ops *[256]string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYstb(oriStm, key, outDecryptName)
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	logln("guessing opcode if not provided...")
	if !guessYstbOp(&script, ops) {
		fmt.Printf("Guess opcodes failed, msg op:0x%x, call op:0x%x\n", IndexOf(ops[:], "msg"), IndexOf(ops[:], "call"))
	}
	if outJsonName != "" {
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
		os.WriteFile(outJsonName, out, os.ModePerm)
	}
	if outInstructName != "" {
		resTypes := map[uint8]string{
			77: "str",
		}
		logln("writing instructions...")
		out := ""
		for i := range script.Insts {
			inst := &script.Insts[i]
			op := ops[int(inst.Op)]
			if op == "" { //!op
				op = strconv.Itoa(int(inst.Op))
			}
			switch op {
			case "msg":
				out += strings.ReplaceAll(resStr(inst.Args[0].Res, codePage), "\"", "") + "\n"
				break
			case "msg-meta":
				out += "msg-data(" + strconv.Itoa(int(inst.Args[0].Value))
				if len(inst.Args[0].Res.ResRaw) != 0 {
					out += ", " + base64.StdEncoding.EncodeToString(inst.Args[0].Res.ResRaw)
				}
				out += ")\n"
				break
			case "call":
				out += "\\" + strings.ReplaceAll(resStr(inst.Args[0].Res, codePage), "\"", "")
				out += "("
				for i := 1; i < len(inst.Args); i++ {
					arg := &inst.Args[i]
					out += strconv.Itoa(int(arg.Value)) + ": " + strconv.Itoa(int(arg.Type)) + " ->"
					writeType := true
					resType, ok := resTypes[arg.Res.Type]
					if !ok {
						resType = strconv.Itoa(int(arg.Res.Type))
					}
					if resType == "str" {
						writeType = false
						resS := resStr(arg.Res, codePage)
						if resS != "''" {
							out += resS
						} else {
							out += "null"
						}
					} else {
						if len(arg.Res.ResRaw) != 0 {
							out += base64.StdEncoding.EncodeToString(arg.Res.ResRaw)
						} else if len(arg.Res.Res) != 0 {
							out += base64.StdEncoding.EncodeToString(arg.Res.Res)
						} else if arg.ResOffset != 0 && arg.ResInfo != 0 {
							out += fmt.Sprintf("res::(%v--%v)", arg.ResInfo, arg.ResOffset)
						} else {
							out += "~"
						}
					}
					if writeType {
						out += ":" + resType
					}
					if i+1 < len(inst.Args) {
						out += ", "
					}
				}
				out += ")\n"
				break
			default:
				out += "\\" + op
				out += "("
				for i := 0; i < len(inst.Args); i++ {
					arg := &inst.Args[i]
					out += strconv.Itoa(int(arg.Value)) + ": " + strconv.Itoa(int(arg.Type)) + " ->"
					writeType := true
					resType, ok := resTypes[arg.Res.Type]
					if !ok {
						resType = strconv.Itoa(int(arg.Res.Type))
					}
					if resType == "str" {
						writeType = false
						resS := resStr(arg.Res, codePage)
						if resS != "''" {
							out += resS
						} else {
							out += "null"
						}
					} else {
						if len(arg.Res.ResRaw) != 0 {
							out += base64.StdEncoding.EncodeToString(arg.Res.ResRaw)
						} else if len(arg.Res.Res) != 0 {
							out += base64.StdEncoding.EncodeToString(arg.Res.Res)
						} else if arg.ResOffset != 0 && arg.ResInfo != 0 {
							out += fmt.Sprintf("res::(%v--%v)", arg.ResInfo, arg.ResOffset)
						} else {
							out += "~"
						}
					}
					if writeType {
						out += ":" + resType
					}
					if i+1 < len(inst.Args) {
						out += ", "
					}
				}
				out += ")\n"
			}
		}
		os.WriteFile(outInstructName, []byte(out), os.ModePerm)
	}
	if outTxtName != "" {
		logln("extracting text from script...")
		txt, err := extTxtFromYbn(&script, ops, codePage)
		if err != nil {
			fmt.Println("error when extracting txt:", err)
			return false
		}
		if len(txt) > 0 {
			logln("encoding text and writing...")
			out := codec.Encode(strings.Join(txt, "\r\n"), codec.UTF8Sig, codec.Replace)
			os.WriteFile(outTxtName, out, os.ModePerm)
		} else {
			logln("no extracted text...")
		}
	}
	logln("complete.")
	return true
}

func parseYstb(oriStm []byte, key []byte, decryptName string) (script ystbInfo, err error) {
	stm := bytes.NewReader(oriStm)
	binary.Read(stm, binary.LittleEndian, &script.Header)
	logln("header:", script.Header)
	header := &script.Header
	if bytes.Compare(header.Meta.Magic[:], []byte("YSTB")) != 0 || header.CodeSize != header.InstCnt*4 {
		err = fmt.Errorf("not a ybn file or file format error")
		return
	}
	fileSize, _ := stm.Seek(0, io.SeekEnd)
	if uint32(binary.Size(header))+header.CodeSize+header.ArgSize+header.ResourceSize+header.OffSize != uint32(fileSize) {
		err = fmt.Errorf("file size error")
		return
	}
	if header.Resv != 0 {
		fmt.Println("reserved is not 0, maybe can't extract all the info")
	}
	var decryptedStm io.ReadSeeker
	if bytes.Compare(key, []byte("\x00\x00\x00\x00")) != 0 {
		logf("decrypting, key is:%02x %02x %02x %02x\n", key[0], key[1], key[2], key[3])
		decryptYstb(oriStm, key, header)
		decryptedStm = bytes.NewReader(oriStm)
	} else {
		decryptedStm = bytes.NewReader(oriStm)
	}
	if decryptName != "" {
		logln("write decrypted file...")
		err1 := os.WriteFile(decryptName, oriStm, 0644)
		fmt.Println(err1)
	}

	logln("reading sections...")
	decryptedStm.Seek(int64(binary.Size(header)), io.SeekStart)
	script.Insts = make([]ystbInstInfo, header.InstCnt)
	rawInsts := make([]ystbInst, header.InstCnt)
	binary.Read(decryptedStm, binary.LittleEndian, &rawInsts)
	var tArg ystbArg
	rargs := make([]ystbArg, header.ArgSize/uint32(binary.Size(tArg)))
	binary.Read(decryptedStm, binary.LittleEndian, &rargs)
	resStartOff, _ := decryptedStm.Seek(0, io.SeekCurrent)
	rargIdx := 0
	logln("parsing instructions...")
	for i, rinst := range rawInsts {
		inst := &script.Insts[i]
		inst.Op = rinst.Op
		inst.LabelId = rinst.LabelId
		inst.Args = make([]ystbArgInfo, rinst.ArgCnt)
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
				decryptedStm.Seek(resStartOff+int64(rarg.ResOffset), io.SeekStart)
				res := &inst.Args[j].Res
				if rarg.Type == 3 {
					var resInfo ystbResInfo
					binary.Read(decryptedStm, binary.LittleEndian, &resInfo)
					res.Type = resInfo.Type
					res.Res = make([]byte, resInfo.Len)
					decryptedStm.Read(res.Res)
				} else {
					if rarg.ResSize > 3 {
						var resInfo ystbResInfo
						binary.Read(decryptedStm, binary.LittleEndian, &resInfo)
						if uint32(resInfo.Len)+3 == rarg.ResSize {
							res.Type = resInfo.Type
							res.Res = make([]byte, resInfo.Len)
							decryptedStm.Read(res.Res)
						} else {
							decryptedStm.Seek(-3, io.SeekCurrent)
							res.ResRaw = make([]byte, rarg.ResSize)
							decryptedStm.Read(res.ResRaw)
						}
					} else {
						res.ResRaw = make([]byte, rarg.ResSize)
						decryptedStm.Read(res.ResRaw)
					}
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
