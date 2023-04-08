package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/textFile"
	"io"
	"os"
	"strconv"
	"strings"
)

type yscmHeader struct {
	Meta  YbnHeader
	Count uint32
	Unk   uint32 //zero
}
type yscmInfo struct {
	Header        yscmHeader
	Commands      []yscmCommandInfo
	ErrorOffset   int64
	ErrorMessages []string //length: 37?
	Unk           []byte
}

type yscmCommandInfo struct {
	Name    string
	Actions []yscmCommandActionInfo
}

type yscmCommandActionInfo struct {
	Name    string
	ArgType byte
	ArgVaid byte
}

func parseYscm(oriStm []byte, codePage int) (script yscmInfo, err error) {
	stm := bytes.NewReader(oriStm)
	binary.Read(stm, binary.LittleEndian, &script.Header)
	logln("header:", script.Header)
	header := &script.Header
	if bytes.Compare(header.Meta.Magic[:], []byte("YSCM")) != 0 {
		err = fmt.Errorf("not a ybn file")
		return
	}
	script.Commands = make([]yscmCommandInfo, script.Header.Count)
	for i := 0; i < int(script.Header.Count); i++ {
		cmd := &script.Commands[i]
		cmd.Name = readAnsiStr(stm, codePage)
		var actionCount uint8
		binary.Read(stm, binary.LittleEndian, &actionCount)
		cmd.Actions = make([]yscmCommandActionInfo, actionCount)
		for j := 0; j < int(actionCount); j++ {
			act := &cmd.Actions[j]
			act.Name = readAnsiStr(stm, codePage)
			binary.Read(stm, binary.LittleEndian, &act.ArgType)
			binary.Read(stm, binary.LittleEndian, &act.ArgVaid)
		}
	}
	pos, err := stm.Seek(0, io.SeekCurrent)
	if err != nil {
		return yscmInfo{}, err
	}
	script.ErrorOffset = pos
	script.ErrorMessages = make([]string, 37)
	for i := 0; i < len(script.ErrorMessages); i++ {
		script.ErrorMessages[i] = readAnsiStr(stm, codePage)
	}
	script.Unk = make([]byte, 256)
	stm.Read(script.Unk)
	return
}

func parseYscmFile(oriStm []byte, outJsonName, outTxtName, outInstructName string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYscm(oriStm, codePage)
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
		for i := range script.Commands {
			cmd := &script.Commands[i]
			out += cmd.Name + "("
			for j := range cmd.Actions {
				act := &cmd.Actions[j]
				out += act.Name + "(" + strconv.Itoa(int(act.ArgType)) + "," + strconv.Itoa(int(act.ArgVaid)) + "),"
			}
			strings.TrimRight(out, ",")
			out += ")\n"
		}
		strings.TrimRight(out, "\n")
		os.WriteFile(outInstructName, []byte(out), os.ModePerm)
	}
	if outTxtName != "" {
		logln("extracting text from script...")
		txt := make([]string, len(script.ErrorMessages))
		for i, msg := range script.ErrorMessages {
			txt[i] = msg
		}
		logln("encoding text and writing...")
		out := codec.Encode(strings.Join(txt, "\r\n"), codec.UTF8Sig, codec.Replace)
		os.WriteFile(outTxtName, out, os.ModePerm)
	}
	logln("complete.")
	return true
}

func packYscmFile(oriStm []byte, outTxtName, outYbnName string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYscm(oriStm, codePage)
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	if outTxtName != "" {
		logln("loading files...")
		ls, err := textFile.ReadWin32TxtToLines(outTxtName)
		if err != nil {
			fmt.Println(err)
			return false
		}
		logln("encoding text and writing...")
		var buffer bytes.Buffer
		buffer.Write(oriStm[:script.ErrorOffset])
		for i := range ls {
			buffer.Write(codec.Encode(ls[i], codePage, codec.Replace))
			buffer.WriteByte(0)
		}
		buffer.Write(script.Unk)
		os.WriteFile(outYbnName, buffer.Bytes(), os.ModePerm)
	}
	logln("complete.")
	return true
}
