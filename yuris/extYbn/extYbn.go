package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"os"
)

type resourceEntry struct {
	Type     uint8
	CodePage uint32
	Res      []byte
}

type argInfo struct {
	Value uint16
	Type  uint16
	Res   resourceEntry
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

func parseYbn(stm io.ReadSeeker) (script ybnInfo, err error) {
	binary.Read(stm, binary.LittleEndian, &script.Header)
	header := &script.Header
	if bytes.Compare(header.Magic[:], []byte("YSTB")) != 0 ||
		header.CodeSize != header.InstCnt*4 ||
		header.ArgSize != header.InstCnt*12 {
		err = fmt.Errorf("not a ybn file or file format error")
		return
	}
	if header.Resv != 0 {
		fmt.Println("reserved is not 0, maybe can't extract all the info")
	}

}

func parseYbnFile(ybnName, outScriptName, outTxtName string, codePage int) bool {
	fs, err := os.Open(ybnName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	defer fs.Close()

}

func main() {
	fmt.Println("abc")
}
