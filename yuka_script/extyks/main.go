package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"

	"github.com/regomne/eutil/textFile"

	"github.com/regomne/eutil/codec"
)

type YksHeader struct {
	Magic      [6]byte
	Unk1       uint16
	HeaderSize uint32
	Unk2       uint32
	PrgOffset  uint32
	PrgCnt     uint32
	CmdOffset  uint32
	CmdCnt     uint32
	ResOffset  uint32
	ResLength  uint32
	Unk3       uint32
	Unk4       uint32
}

type CmdInfo struct {
	Op  uint32
	Arg [3]int32
}

type cmdRecord struct {
	Name string
	Args interface{}
}

type cmdColRecord struct {
	A1 []byte
	A2 int32
	A3 int32
	A4 int32
}

type cmdLblRecord struct {
	A1 []byte
	A2 int32
}

type cmdI32Record struct {
	A1 int32
}

type cmdStrRecord struct {
	A1 []byte
}

type cmdFlgRecord = cmdLblRecord

type cmdOp9Record = cmdFlgRecord

type cmdOp10Record = cmdI32Record

type yksRecord struct {
	Header YksHeader
	Cmd    []cmdRecord
}

func getStr(res []byte, off int32) (s []byte) {
	idx := bytes.IndexByte(res[off:], 0)
	if idx == -1 {
		s = res[off:]
	} else {
		s = res[off : int(off)+idx]
	}
	return
}

func getInt32(res []byte, off int32) (v int32) {
	r := bytes.NewReader(res[off:])
	binary.Read(r, binary.LittleEndian, &v)
	return
}

func parseYks(stm io.ReadSeeker) (script yksRecord, err error) {
	binary.Read(stm, binary.LittleEndian, &script.Header)
	header := &script.Header
	if bytes.Compare(header.Magic[:], []byte("YKS001")) != 0 {
		err = fmt.Errorf("not a YKS file or unsupported version")
		return
	}
	if header.HeaderSize != uint32(binary.Size(header)) {
		err = fmt.Errorf("maybe a corrupted YKS file")
		return
	}
	prgs := make([]uint32, header.PrgCnt)
	stm.Seek(int64(header.PrgOffset), 0)
	binary.Read(stm, binary.LittleEndian, &prgs)
	cmds := make([]CmdInfo, header.CmdCnt)
	stm.Seek(int64(header.CmdOffset), 0)
	binary.Read(stm, binary.LittleEndian, &cmds)
	stm.Seek(int64(header.ResOffset), 0)
	res := make([]byte, header.ResLength)
	stm.Read(res)

	script.Cmd = make([]cmdRecord, 0, header.PrgCnt)

	for idx := 0; idx < len(prgs); {
		cmd := cmds[prgs[idx]]
		var rec cmdRecord
		switch cmd.Op {
		case 0:
			var arg cmdColRecord
			arg.A1 = getStr(res, cmd.Arg[0])
			arg.A2 = cmd.Arg[1]
			arg.A3 = cmd.Arg[2]
			arg.A4 = int32(prgs[idx+1])
			idx++
			rec.Name = "COL"
			rec.Args = arg
		case 1:
			var arg cmdLblRecord
			arg.A1 = getStr(res, cmd.Arg[0])
			arg.A2 = getInt32(res, cmd.Arg[1])
			if cmd.Arg[2] != -1 {
				err = fmt.Errorf("Op Lbl seems error in prg:%d", idx)
				return
			}
			rec.Name = "LBL"
			rec.Args = arg
		case 4:
			var arg cmdI32Record
			arg.A1 = getInt32(res, cmd.Arg[1])
			if cmd.Arg[0] != 0 || cmd.Arg[2] != 0 {
				err = fmt.Errorf("Op U32 seems error in prg:%d", idx)
				return
			}
			rec.Name = "U32"
			rec.Args = arg
		case 5:
			var arg cmdStrRecord
			arg.A1 = getStr(res, cmd.Arg[1])
			if cmd.Arg[0] != 0 || cmd.Arg[2] != 0 {
				err = fmt.Errorf("Op STR seems error in prg:%d", idx)
				return
			}
			rec.Name = "STR"
			rec.Args = arg
		case 8:
			var arg cmdFlgRecord
			arg.A1 = getStr(res, cmd.Arg[0])
			arg.A2 = getInt32(res, cmd.Arg[2])
			if cmd.Arg[1] != 0 {
				err = fmt.Errorf("Op FLG seems error in prg:%d", idx)
				return
			}
			rec.Name = "FLG"
			rec.Args = arg
		case 9:
			var arg cmdOp9Record
			arg.A1 = getStr(res, cmd.Arg[0])
			arg.A2 = getInt32(res, cmd.Arg[2])
			if cmd.Arg[1] != 0 {
				err = fmt.Errorf("Op OP9 seems error in prg:%d", idx)
				return
			}
			rec.Name = "OP9"
			rec.Args = arg
		case 10:
			var arg cmdOp10Record
			arg.A1 = cmd.Arg[1]
			if cmd.Arg[0] != 0 || cmd.Arg[2] != 0 {
				err = fmt.Errorf("Op OP10 seems error in prg:%d", idx)
				return
			}
			rec.Name = "OP10"
			rec.Args = arg
		default:
			err = fmt.Errorf("unknown cmd op: %d in prg:%d", cmd.Op, idx)
			return
		}
		idx++
		script.Cmd = append(script.Cmd, rec)
	}
	return
}

func encodeYks(out io.Writer, yks yksRecord) (err error) {
	prgs := make([]uint32, 0, len(yks.Cmd))
	cmds := make([]CmdInfo, 0, len(yks.Cmd))
	var res bytes.Buffer
	strTable := make(map[string]int32)
	intTable := make(map[int32]int32)
	cmdTable := make(map[CmdInfo]uint32)
	makeStr := func(s []byte) int32 {
		off, ok := strTable[string(s)]
		if ok {
			return off
		}
		off = int32(res.Len())
		strTable[string(s)] = off
		res.Write(s)
		res.WriteByte(0)
		return off
	}
	makeInt := func(i int32) int32 {
		off, ok := intTable[i]
		if ok {
			return off
		}
		off = int32(res.Len())
		intTable[i] = off
		binary.Write(&res, binary.LittleEndian, i)
		return off
	}
	makeCmd := func(info *CmdInfo) uint32 {
		off, ok := cmdTable[*info]
		if ok {
			return off
		}
		off = uint32(len(cmds))
		cmdTable[*info] = off
		cmds = append(cmds, *info)
		return off
	}
	for _, cmd := range yks.Cmd {
		info := CmdInfo{}
		switch cmd.Name {
		case "COL":
			arg, _ := cmd.Args.(cmdColRecord)
			info.Arg[0] = makeStr(arg.A1)
			info.Arg[1] = arg.A2
			info.Arg[2] = arg.A3
			info.Op = 0
			prgs = append(prgs, makeCmd(&info))
			prgs = append(prgs, uint32(arg.A4))
		case "LBL":
			arg, _ := cmd.Args.(cmdLblRecord)
			info.Arg[0] = makeStr(arg.A1)
			info.Arg[1] = makeInt(arg.A2)
			info.Arg[2] = -1
			info.Op = 1
			prgs = append(prgs, makeCmd(&info))
		case "U32":
			arg, _ := cmd.Args.(cmdI32Record)
			info.Arg[1] = makeInt(arg.A1)
			info.Op = 4
			prgs = append(prgs, makeCmd(&info))
		case "STR":
			arg, _ := cmd.Args.(cmdStrRecord)
			info.Arg[1] = makeStr(arg.A1)
			info.Op = 5
			prgs = append(prgs, makeCmd(&info))
		case "FLG":
			arg, _ := cmd.Args.(cmdFlgRecord)
			info.Arg[0] = makeStr(arg.A1)
			info.Arg[2] = makeInt(arg.A2)
			info.Op = 8
			prgs = append(prgs, makeCmd(&info))
		case "OP9":
			arg, _ := cmd.Args.(cmdOp9Record)
			info.Arg[0] = makeStr(arg.A1)
			info.Arg[2] = makeInt(arg.A2)
			info.Op = 9
			prgs = append(prgs, makeCmd(&info))
		case "OP10":
			arg, _ := cmd.Args.(cmdOp10Record)
			info.Arg[1] = arg.A1
			info.Op = 10
			prgs = append(prgs, makeCmd(&info))
		default:
			err = fmt.Errorf("unknown cmd name:%s", cmd.Name)
			return
		}
	}
	newHeader := yks.Header
	newHeader.PrgCnt = uint32(len(prgs))
	newHeader.PrgOffset = uint32(binary.Size(newHeader))
	newHeader.CmdCnt = uint32(len(cmds))
	newHeader.CmdOffset = newHeader.PrgOffset + 4*newHeader.PrgCnt
	newHeader.ResLength = uint32(res.Len())
	newHeader.ResOffset = newHeader.CmdOffset + 16*newHeader.CmdCnt
	binary.Write(out, binary.LittleEndian, &newHeader)
	binary.Write(out, binary.LittleEndian, &prgs)
	binary.Write(out, binary.LittleEndian, &cmds)
	out.Write(res.Bytes())
	return
}

func extTextFromYksRecord(yks yksRecord, codePage int) (txt []string, err error) {
	txt = make([]string, 0, len(yks.Cmd)/3)
	for i := 0; i < len(yks.Cmd); i++ {
		cmd := &yks.Cmd[i]
		if cmd.Name == "COL" {
			arg, _ := cmd.Args.(cmdColRecord)
			if bytes.Compare(arg.A1, []byte("StrOutNW")) == 0 {
				cmd = &yks.Cmd[i+2]
				if cmd.Name != "STR" {
					err = fmt.Errorf("StrOutNW not followed with str, cmd idx:%d", i)
					return
				}
				arg1, _ := cmd.Args.(cmdStrRecord)
				txt = append(txt, codec.Decode(arg1.A1, codePage))
				i += 2
			} else if bytes.Compare(arg.A1, []byte("StrOut")) == 0 {
				cmd = &yks.Cmd[i+1]
				if cmd.Name != "STR" {
					err = fmt.Errorf("StrOut not followed with str, cmd idx:%d", i)
					return
				}
				arg1, _ := cmd.Args.(cmdStrRecord)
				txt = append(txt, codec.Decode(arg1.A1, codePage))
				i++
			}
		} else if cmd.Name == "OP9" {
			cmd = &yks.Cmd[i+2]
			if cmd.Name != "STR" {
				err = fmt.Errorf("OP9 not followed with str, cmd idx:%d", i)
				return
			}
			arg, _ := cmd.Args.(cmdStrRecord)
			txt = append(txt, codec.Decode(arg.A1, codePage))
			i += 2
		}
	}
	return
}

func packTextToYksRecord(yks yksRecord, txt []string, codePage int) (err error) {
	txtIdx := 0
	for i := 0; i < len(yks.Cmd); i++ {
		cmd := &yks.Cmd[i]
		if cmd.Name == "COL" {
			arg, _ := cmd.Args.(cmdColRecord)
			if bytes.Compare(arg.A1, []byte("StrOutNW")) == 0 {
				cmd = &yks.Cmd[i+2]
				if cmd.Name != "STR" {
					err = fmt.Errorf("StrOutNW not followed with str, cmd idx:%d", i)
					return
				}
				arg1, _ := cmd.Args.(cmdStrRecord)
				if txtIdx >= len(txt) {
					err = fmt.Errorf("txt lines error")
					return
				}
				arg1.A1 = codec.Encode(txt[txtIdx], codePage, codec.Replace)
				cmd.Args = arg1
				i += 2
				txtIdx++
			} else if bytes.Compare(arg.A1, []byte("StrOut")) == 0 {
				cmd = &yks.Cmd[i+1]
				if cmd.Name != "STR" {
					err = fmt.Errorf("StrOut not followed with str, cmd idx:%d", i)
					return
				}
				arg1, _ := cmd.Args.(cmdStrRecord)
				if txtIdx >= len(txt) {
					err = fmt.Errorf("txt lines error")
					return
				}
				arg1.A1 = codec.Encode(txt[txtIdx], codePage, codec.Replace)
				cmd.Args = arg1
				i++
				txtIdx++
			}
		} else if cmd.Name == "OP9" {
			cmd = &yks.Cmd[i+2]
			if cmd.Name != "STR" {
				err = fmt.Errorf("OP9 not followed with str, cmd idx:%d", i)
				return
			}
			arg, _ := cmd.Args.(cmdStrRecord)
			if txtIdx >= len(txt) {
				err = fmt.Errorf("txt lines error")
				return
			}
			arg.A1 = codec.Encode(txt[txtIdx], codePage, codec.Replace)
			cmd.Args = arg
			i += 2
			txtIdx++
		}
	}
	if txtIdx != len(txt) {
		err = fmt.Errorf("txt lines error")
		return
	}
	return
}

func writeByteLines(w io.Writer, ls [][]byte, inCP int, outCP int) {
	if inCP == outCP {
		w.Write(bytes.Join(ls, []byte{'\r', '\n'}))
		return
	}
	nls := make([][]byte, len(ls))
	for i, l := range ls {
		s := codec.Decode(l, inCP)
		nls[i] = codec.Encode(s, outCP, codec.Replace)
	}
	w.Write(bytes.Join(nls, []byte{'\r', '\n'}))
	return
}

func parseYksFile(yksFileName, outScriptName, outTxtName string, codePage int) bool {
	fs, err := os.Open(yksFileName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	defer fs.Close()
	script, err := parseYks(fs)
	if err != nil {
		fmt.Println("parsing failed:", err)
		return false
	}
	if outScriptName != "" {
		os.MkdirAll(filepath.Dir(outScriptName), os.ModePerm)
		out, err := os.Create(outScriptName)
		if err != nil {
			fmt.Println(err)
			return false
		}
		content, err := json.MarshalIndent(script, "", "  ")
		//writeByteLines(out, scLines, codePage, codec.UTF8)
		out.Write(content)
		out.Close()
	}

	if outTxtName != "" {
		os.MkdirAll(filepath.Dir(outTxtName), os.ModePerm)
		out, err := os.Create(outTxtName)
		if err != nil {
			fmt.Println(err)
			return false
		}
		txt, err := extTextFromYksRecord(script, codePage)
		out.Write(codec.Encode(strings.Join(txt, "\r\n"), codec.UTF8Sig, codec.Replace))
		out.Close()
	}
	return true
}

func packTxtToYksFile(yksFileName, txtFileName, outYksName string, codePage int) (ret bool) {
	ret = false
	fs, err := os.Open(yksFileName)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer fs.Close()
	script, err := parseYks(fs)
	if err != nil {
		fmt.Println("parsing failed:", err)
		return
	}
	ls, err := textFile.ReadWin32TxtToLines(txtFileName)
	if err != nil {
		fmt.Println("reading text failed:", err)
		return
	}
	err = packTextToYksRecord(script, ls, codePage)
	if err != nil {
		fmt.Println("packing text to yks fail:", err)
		return
	}
	os.MkdirAll(filepath.Dir(outYksName), os.ModePerm)
	nfs, err := os.Create(outYksName)
	if err != nil {
		fmt.Printf("creating %s fail: %v\n", outYksName, err)
		return
	}
	defer nfs.Close()
	err = encodeYks(nfs, script)
	if err != nil {
		fmt.Println("encoding yks fail:", err)
		return
	}
	return true
}

func writeYksFile(oriname string, outname string) bool {
	fs, err := os.Open(oriname)
	if err != nil {
		fmt.Println(err)
		return false
	}
	defer fs.Close()
	script, err := parseYks(fs)
	if err != nil {
		fmt.Println("parsing failed:", err)
		return false
	}
	nfs, err := os.Create(outname)
	if err != nil {
		fmt.Println("create file failed", err)
		return false
	}
	defer nfs.Close()
	err = encodeYks(nfs, script)
	if err != nil {
		fmt.Println("encode error:", err)
		return false
	}
	return true
}

func usage() {
	fmt.Printf(`Usage:
  %s -e -yks=in.yks [-json=out.json] [-txt=out.txt] [-cp=XXX]
  %s -p <-yks=in.yks|-json=in.json> <-txt=in.txt> <-new-yks=out.yks> [-cp=XXX]
`, os.Args[0], os.Args[0])
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
	exitCode := 0
	defer func() {
		os.Exit(exitCode)
	}()
	isExt := flag.Bool("e", false, "extract txt from yks")
	isPack := flag.Bool("p", false, "pack txt to yks")
	jsonName := flag.String("json", "", "json file name")
	txtName := flag.String("txt", "", "txt file name")
	inYksName := flag.String("yks", "", "yks file name")
	outYksName := flag.String("new-yks", "", "new yks file name")
	cp := flag.String("cp", "932", "code page in yks(only 932 or 936)")
	flag.Parse()
	if (!*isExt && !*isPack) || (*isExt && *isPack) ||
		(*isExt && *inYksName == "") ||
		(*isPack && ((*jsonName == "" && *inYksName == "") || *txtName == "" || *outYksName == "")) {
		usage()
		return
	}
	codePage := parseCp(*cp)
	if codePage == codec.Unknown {
		fmt.Println("not supported code page")
		return
	}
	if *isExt {
		if !parseYksFile(*inYksName, *jsonName, *txtName, codePage) {
			exitCode = 1
		}
	} else {
		if *inYksName != "" && *jsonName != "" {
			fmt.Println("input is yks or json, not both")
			return
		}
		if *inYksName == "" {
			fmt.Println("not support json input now")
			return
		}
		if !packTxtToYksFile(*inYksName, *txtName, *outYksName, codePage) {
			exitCode = 1
		}
	}
}
