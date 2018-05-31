package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"os"

	"github.com/regomne/eutil/codec"
)

type resourceEntry struct {
	Type   uint8
	Res    []byte
	ResStr string `json:",omitempty"`
}

type argInfo struct {
	Value uint16
	Type  uint16
	Res   resourceEntry
}

type instInfo = YInst

type ybnInfo struct {
	Header YbnHeader
	Insts  []instInfo
	Args   []argInfo
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
		header.CodeSize != header.InstCnt*4 ||
		header.ArgSize != header.InstCnt*12 {
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
	binary.Read(stm, binary.LittleEndian, &script.Insts)
	args := make([]YArg, header.InstCnt)
	binary.Read(stm, binary.LittleEndian, &args)
	resStartOff, _ := stm.Seek(0, 1)
	script.Args = make([]argInfo, header.InstCnt)
	for i, arg := range args {
		script.Args[i].Type = arg.Type
		script.Args[i].Value = arg.Value
		stm.Seek(resStartOff+int64(arg.ResOffset), 0)
		var resInfo YResInfo
		res := &script.Args[i].Res
		binary.Read(stm, binary.LittleEndian, &resInfo)
		res.Type = resInfo.Type
		res.Res = make([]byte, resInfo.Len)
		stm.Read(res.Res)
	}
	offTblOffset := uint32(binary.Size(header)) + header.CodeSize + header.ArgSize + header.ResourceSize
	stm.Seek(int64(offTblOffset), 0)
	script.Offs = make([]uint32, header.InstCnt)
	binary.Read(stm, binary.LittleEndian, &script.Offs)
	return
}

func decodeScriptString(script ybnInfo, codePage int) {
	for i := range script.Args {
		res := &script.Args[i].Res
		res.ResStr = codec.Decode(res.Res, codePage)
	}
}

func parseYbnFile(ybnName, outScriptName, outTxtName string, codePage int) bool {
	fs, err := os.Open(ybnName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	defer fs.Close()
	return true
}

func main() {
	fmt.Println("abc")
}
