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

type ystlHeader struct {
	Meta  YbnHeader
	Count uint32
}

type ystlInfo struct {
	Header  ystlHeader
	Scripts []ystlScriptInfo
}

type ystlScriptInfo struct {
	Id           uint32
	SourceLength uint32
	Source       string
	Unk1         uint32
	Unk2         uint32
	Unk3         uint32
	Unk4         uint32
	Unk5         uint32
}

func parseYstlFile(oriStm []byte, outJsonName, outInstructName string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYstl(oriStm, codePage)
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
		for i := range script.Scripts {
			scr := &script.Scripts[i]
			out += fmt.Sprintf("yst%05d.ybn => %s  (%v,%v,%v,%v,%v)\n", scr.Id, scr.Source, scr.Unk1, scr.Unk2, scr.Unk3, scr.Unk4, scr.Unk5)
		}
		strings.TrimRight(out, "\n")
		os.WriteFile(outInstructName, []byte(out), os.ModePerm)
	}
	logln("complete.")
	return true
}

func parseYstl(oriStm []byte, codePage int) (script ystlInfo, err error) {
	stm := bytes.NewReader(oriStm)
	binary.Read(stm, binary.LittleEndian, &script.Header)
	logln("header:", script.Header)
	header := &script.Header
	if bytes.Compare(header.Meta.Magic[:], []byte("YSTL")) != 0 {
		err = fmt.Errorf("not a ybn file")
		return
	}
	script.Scripts = make([]ystlScriptInfo, script.Header.Count)
	for i := 0; i < int(script.Header.Count); i++ {
		scr := &script.Scripts[i]
		binary.Read(stm, binary.LittleEndian, &scr.Id)
		var sourceLength uint32
		binary.Read(stm, binary.LittleEndian, &sourceLength)
		encodedName := make([]byte, sourceLength)
		stm.Read(encodedName)
		scr.Source = codec.Decode(encodedName, codePage)
		binary.Read(stm, binary.LittleEndian, &scr.Unk1)
		binary.Read(stm, binary.LittleEndian, &scr.Unk2)
		binary.Read(stm, binary.LittleEndian, &scr.Unk3)
		binary.Read(stm, binary.LittleEndian, &scr.Unk4)
		binary.Read(stm, binary.LittleEndian, &scr.Unk5)
	}
	return
}
