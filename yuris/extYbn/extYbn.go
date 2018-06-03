package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"strings"

	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/memio"
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
		out := codec.Encode(strings.Join(txt, "\r\n"), codec.UTF8, codec.Replace)
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

func main() {
	ops := keyOps{108, 43}
	parseYbnFile(`d:\chinesize\yuris\ysbind\yst00237.ybn`,
		`d:\chinesize\yuris\yst00237.json`,
		`d:\chinesize\yuris\yst00237.txt`,
		[]byte("\x00\x00\x00\x00"), &ops, codec.C932)
}
