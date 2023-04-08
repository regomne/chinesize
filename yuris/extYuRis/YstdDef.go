package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"os"
)

type ystdHeader struct {
	Meta  YbnHeader
	Count uint32
	Unk   uint32
}

type ystdInfo struct {
	Header ystdHeader
}

func parseYstdFile(oriStm []byte, outJsonName, outInstructName string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYstd(oriStm, codePage)
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	if outJsonName != "" {
		logln("writing json...")
		out, err := json.MarshalIndent(script, "", "\t")
		if err != nil {
			fmt.Println("error when marshalling json:", err)
			return false
		}
		os.WriteFile(outJsonName, out, os.ModePerm)
	}
	if outInstructName != "" {
		logln("writing instructions...")
		out := fmt.Sprintf("YSTD v%v\n%v\n%v", script.Header.Meta.Version, script.Header.Count, script.Header.Unk)
		os.WriteFile(outInstructName, []byte(out), os.ModePerm)
	}
	logln("complete.")
	return true
}

func parseYstd(oriStm []byte, codePage int) (script ystdInfo, err error) {
	stm := bytes.NewReader(oriStm)
	binary.Read(stm, binary.LittleEndian, &script.Header)
	logln("header:", script.Header)
	header := &script.Header
	if bytes.Compare(header.Meta.Magic[:], []byte("YSTD")) != 0 {
		err = fmt.Errorf("not a ybn file")
		return
	}
	return
}
