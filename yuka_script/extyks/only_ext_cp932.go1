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
	A1 string
	A2 int32
	A3 int32
	A4 int32
}

type cmdLblRecord struct {
	A1 string
	A2 int32
}

type cmdI32Record struct {
	A1 int32
}

type cmdStrRecord struct {
	A1 string
}

type cmdFlgRecord = cmdLblRecord

type cmdOp9Record = cmdFlgRecord

type cmdOp10Record = cmdI32Record

type yksRecord struct {
	Header YksHeader
	Cmd    []cmdRecord
}

func getStr(res []byte, off int32) (s string) {
	idx := bytes.IndexByte(res[off:], 0)
	var b []byte
	if idx == -1 {
		b = res[off:]
	} else {
		b = res[off : int(off)+idx]
	}
	s = codec.Decode(b, codec.C932)
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
	}
}
