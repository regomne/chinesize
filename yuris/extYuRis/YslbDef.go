package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"github.com/regomne/eutil/codec"
	"os"
	"strings"
)

type yslbHeader struct {
	Meta  YbnHeader
	Count uint32
	Unk   [256]uint32
}

type yslbInfo struct {
	Header yslbHeader
	Labels []yslbLabel
}

type yslbLabel struct {
	Name         string
	NameHash     uint32
	CommandIndex uint32
	ScriptId     uint16
	Unk1         byte
	Unk2         byte
}

func parseYslbFile(oriStm []byte, outJsonName, outInstructName string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYslb(oriStm, codePage)
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
		out := ""
		for i := range script.Labels {
			label := &script.Labels[i]
			out += fmt.Sprintf("#=\"%s\" =>yst%05d.ybn.instruct:%5d\n", label.Name, label.ScriptId, label.CommandIndex)
		}
		strings.TrimRight(out, "\n")
		os.WriteFile(outInstructName, []byte(out), os.ModePerm)
	}
	logln("complete.")
	return true
}

func parseYslb(oriStm []byte, codePage int) (script yslbInfo, err error) {
	stm := bytes.NewReader(oriStm)
	binary.Read(stm, binary.LittleEndian, &script.Header)
	logln("header:", script.Header)
	header := &script.Header
	if bytes.Compare(header.Meta.Magic[:], []byte("YSLB")) != 0 {
		err = fmt.Errorf("not a ybn file")
		return
	}
	script.Labels = make([]yslbLabel, script.Header.Count)
	for i := 0; i < int(script.Header.Count); i++ {
		label := &script.Labels[i]
		var nameLength uint8
		binary.Read(stm, binary.LittleEndian, &nameLength)
		encodedName := make([]byte, nameLength)
		stm.Read(encodedName)
		label.Name = codec.Decode(encodedName, codePage)
		binary.Read(stm, binary.LittleEndian, &label.NameHash)
		binary.Read(stm, binary.LittleEndian, &label.CommandIndex)
		binary.Read(stm, binary.LittleEndian, &label.ScriptId)
		binary.Read(stm, binary.LittleEndian, &label.Unk1)
		binary.Read(stm, binary.LittleEndian, &label.Unk2)
	}
	return
}
