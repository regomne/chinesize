package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"github.com/regomne/eutil/codec"
	"os"
	"regexp"
	"strconv"
	"strings"
)

type yserHeader struct {
	Meta  YbnHeader
	Count uint32
	Unk   uint32 //zero
}

type yserInfo struct {
	Header        yserHeader
	ErrorMessages []yserErrorMessage
}

type yserErrorMessage struct {
	Code    uint32
	Message string
}

func parseYserFile(oriStm []byte, outJsonName, outTxtName string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYser(oriStm, codePage)
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
	if outTxtName != "" {
		logln("writing instructions...")
		out := ""
		for i := range script.ErrorMessages {
			msg := &script.ErrorMessages[i]
			out += strconv.Itoa(int(msg.Code)) + "->\"" + msg.Message + "\"\n"
		}
		strings.TrimRight(out, "\n")
		os.WriteFile(outTxtName, []byte(out), os.ModePerm)
	}
	logln("complete.")
	return true
}

func parseYser(oriStm []byte, codePage int) (script yserInfo, err error) {
	stm := bytes.NewReader(oriStm)
	binary.Read(stm, binary.LittleEndian, &script.Header)
	logln("header:", script.Header)
	header := &script.Header
	if bytes.Compare(header.Meta.Magic[:], []byte("YSER")) != 0 {
		err = fmt.Errorf("not a ybn file")
		return
	}
	script.ErrorMessages = make([]yserErrorMessage, script.Header.Count)
	for i := 0; i < int(script.Header.Count); i++ {
		msg := &script.ErrorMessages[i]
		binary.Read(stm, binary.LittleEndian, &msg.Code)
		msg.Message = readAnsiStr(stm, codePage)
	}
	return
}

func packYserFile(oriStm []byte, outTxtName, outYbnName string, codePage int) bool {
	logln("parsing ybn...")
	script, err := parseYser(oriStm, codePage)
	if err != nil {
		fmt.Println("parse error:", err)
		return false
	}
	if outTxtName != "" {
		logln("loading files...")
		txt, err := readFileToString(outTxtName, codePage)
		if err != nil {
			fmt.Println(err)
			return false
		}
		logln("encoding text and writing...")
		var buffer bytes.Buffer
		buffer.Write(oriStm[:binary.Size(script.Header)])
		var writer BufferWriter
		writer.Buffer = &buffer
		reg, err := regexp.Compile("(?:^|\\n)([0-9]+)->\"([^\"]+)\"")
		if err != nil {
			fmt.Println(err)
			return false
		}
		matches := reg.FindAllStringSubmatch(txt, -1)
		for i := range matches {
			var code, err = strconv.Atoi(matches[i][1])
			if err != nil {
				fmt.Println(err)
				return false
			}
			binary.Write(&writer, binary.LittleEndian, uint32(code))
			buffer.Write(codec.Encode(matches[i][2], codePage, codec.ReplaceHTML))
			fmt.Println(code, matches[i][2])
			buffer.WriteByte(0)
		}
		os.WriteFile(outYbnName, buffer.Bytes(), os.ModePerm)
	}
	logln("complete.")
	return true
}
