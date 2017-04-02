package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"strconv"
	"strings"

	"flag"

	"github.com/MJKWoolnough/memio"
	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/pehelper"
)

var gInstInfo *[256][]byte

func parsePattern(pattern string) (isIgnore []bool, patt []byte) {
	bf := strings.Split(pattern, " ")
	isIgnore = make([]bool, len(bf))
	patt = make([]byte, len(bf))
	for idx, by := range bf {
		if by == "??" {
			isIgnore[idx] = true
		} else {
			b, err := strconv.ParseUint(by, 16, 8)
			if err != nil {
				panic(err)
			}
			patt[idx] = byte(b)
		}
	}
	return
}

func findBytes(buf []byte, pattern string) (uint32, error) {
	isIgnore, patt := parsePattern(pattern)
	for i := 0; i < len(buf)-len(patt); i++ {
		match := true
		for j := 0; j < len(patt); j++ {
			if isIgnore[j] {
				continue
			}
			if patt[j] != buf[i+j] {
				match = false
				break
			}
		}
		if match {
			return uint32(i), nil
		}
	}
	return 0, fmt.Errorf("can't find pattern")
}

func readInstInfo(mem *memio.ReadMem, startOff uint32, base uint32) (instInfo [256][]byte) {
	for i := uint32(0); i < 256; i++ {
		mem.Seek(int64(startOff-base+i*4), 0)
		var np uint32
		binary.Read(mem, binary.LittleEndian, &np)
		buf := bytes.NewBuffer([]byte{})
		if np != 0 {
			mem.Seek(int64(np-base), 0)
			for by, err := mem.ReadByte(); by != 0xff; by, err = mem.ReadByte() {
				if err != nil {
					panic(err)
				}
				buf.WriteByte(by)
			}
			instInfo[i] = buf.Bytes()
		} else {
			instInfo[i] = nil
		}
	}
	return
}

const (
	tByte        = 0
	tShort       = 1
	tWord        = 2
	tLong        = 3
	tDword       = 4
	tFloat       = 5
	tStr         = 6
	tStrDefine   = 9
	tStrFileName = 10
	tVariable    = 7
	tDelimiter   = 8
)

func readUntil(buf io.Reader, b byte) []byte {
	s := []byte{0}
	nbuf := bytes.NewBuffer([]byte{})
	for {
		n, _ := buf.Read(s)
		if n != 1 {
			panic(fmt.Errorf("can't find %d until EOF", b))
		}
		if s[0] != b {
			nbuf.WriteByte(s[0])
		} else {
			return nbuf.Bytes()
		}
	}
}

func getOp(buf *memio.ReadMem, op byte, opNext byte, strCodec int) (val interface{}, text string, retErr error) {
	switch op {
	case tByte:
		b, _ := buf.ReadByte()
		text = fmt.Sprintf("Byte: %d", b)
		val = b
	case tShort:
		var v int16
		binary.Read(buf, binary.LittleEndian, &v)
		text = fmt.Sprintf("Short: %d", v)
		val = v
	case tWord:
		var v uint16
		binary.Read(buf, binary.LittleEndian, &v)
		text = fmt.Sprintf("Word: %d", v)
		val = v
	case tLong:
		var v int32
		binary.Read(buf, binary.LittleEndian, &v)
		text = fmt.Sprintf("Long: %d", v)
		val = v
	case tDword:
		var v uint32
		binary.Read(buf, binary.LittleEndian, &v)
		text = fmt.Sprintf("Dword: %d", v)
		val = v
	case tFloat:
		var v float32
		binary.Read(buf, binary.LittleEndian, &v)
		text = fmt.Sprintf("Float: %g", v)
		val = v
	case tStr:
		s := readUntil(*buf, 0)
		buf.UnreadByte()
		ns := codec.Decode(s, strCodec)
		text = fmt.Sprintf("Str: %s", ns)
		val = ns
	case tStrDefine:
		s := readUntil(*buf, 0)
		buf.UnreadByte()
		ns := codec.Decode(s, strCodec)
		text = fmt.Sprintf("StrDef: %s", ns)
		val = ns
	case tStrFileName:
		s := readUntil(*buf, 0)
		buf.UnreadByte()
		ns := codec.Decode(s, strCodec)
		text = fmt.Sprintf("StrFile: %s", ns)
		val = ns
	case tVariable:
		cnt, _ := buf.ReadByte()
		text2 := []string{}
		for j := 0; j < int(cnt); j++ {
			_, nt, err := getOp(buf, opNext, 255, strCodec)
			if err != nil {
				retErr = err
				return
			}
			if opNext == tStr || opNext == tStrDefine || opNext == tStrFileName {
				buf.Seek(1, 1)
			}
			text2 = append(text2, nt)
		}
		text = fmt.Sprintf("Array cnt:%d, val: %s", cnt, strings.Join(text2, " "))
		val = nil
	case tDelimiter:
		buf.ReadByte()
	default:
		panic(fmt.Errorf("unk op code"))
	}
	return
}

func getSelectItem(buf *memio.ReadMem, strCodec int) (text string, sel string, retErr error) {
	var (
		w1 uint16
		b1 uint8
		w2 uint16
	)
	binary.Read(buf, binary.LittleEndian, &w1)
	s := readUntil(buf, 0)
	sel = codec.Decode(s, codec.ShiftJIS)
	binary.Read(buf, binary.LittleEndian, &b1)
	binary.Read(buf, binary.LittleEndian, &w2)
	op, _ := buf.ReadByte()
	ls, _, err := parseInst(buf, op, gInstInfo[op], strCodec)
	if err != nil {
		retErr = err
		return
	}
	text = fmt.Sprintf("w1:%d, b1:%d, w2:%d, lines:%v, sel:%s",
		w1, b1, w2, ls, sel)
	return
}

func parseInst(buf *memio.ReadMem, oriOp byte, info []byte, strCodec int) (
	text []string, pureTxt []string, retErr error) {
	text = make([]string, 0, len(info))
	pureTxt = make([]string, 0, len(info))
	vals := make([]interface{}, 0, len(info))
	//text = fmt.Sprintf("Op %x, argcnt: %d", oriOp, len(info)))
	if oriOp == 15 {
		if len(info) != 1 {
			retErr = fmt.Errorf("not supported version")
			return
		}
		val, line, err := getOp(buf, info[0], 255, strCodec)
		if err != nil {
			retErr = err
			return
		}
		cnt, ok := val.(byte)
		if !ok {
			retErr = fmt.Errorf("not supported version")
			return
		}
		text = append(text, line)
		for i := byte(0); i < cnt; i++ {
			line, sel, err := getSelectItem(buf, strCodec)
			if err != nil {
				retErr = err
				return
			}
			text = append(text, fmt.Sprintf("%d: %s", i, line))
			pureTxt = append(pureTxt, "Sel:"+sel)
		}
		return
	}
	for i := 0; i < len(info); i++ {
		op := info[i]
		if op == tVariable {
			val, line, err := getOp(buf, op, info[i+1], strCodec)
			if err != nil {
				retErr = err
				return
			}
			i++
			text = append(text, line)
			vals = append(vals, val)
		} else {
			val, line, err := getOp(buf, op, 255, strCodec)
			if err != nil {
				retErr = err
				return
			}
			if line != "" {
				text = append(text, line)
				vals = append(vals, val)
			}
		}
	}

	if oriOp == 0x15 {
		s, _ := vals[0].(string)
		if len(s) != 0 {
			if strings.Index(s, "%LC") == 0 {
				s = s[3:]
			}
			pureTxt = append(pureTxt, s)
		}
	} else if oriOp == 0x14 {
		s, _ := vals[2].(string)
		if strings.LastIndex(s, "%K%P") == len(s)-4 {
			s = s[:len(s)-4]
		}
		pureTxt = append(pureTxt, s)
	}
	return
}

func parseWs2(buf *memio.ReadMem, instInfo *[256][]byte, onlyTxt bool) (text []string, pureTxt []string, retErr error) {
	text = []string{}
	pureTxt = []string{}
	for {
		by, err := buf.ReadByte()
		if err != nil {
			if err.Error() == "EOF" {
				return
			}
			retErr = err
			return
		}
		if by == 255 {
			text = append(text, "End")
			return
		}
		info := instInfo[by]
		pos, _ := buf.Seek(0, 1)
		pos--
		log.Printf("cur op:%d off:0x%x inst:%v\n", by, pos, info)
		if info == nil {
			retErr = fmt.Errorf("unk opcode: %d in off 0x%x", by, pos)
			return
		}
		var optxt []string
		var pTxt []string
		optxt, pTxt, err = parseInst(buf, by, info, codec.ShiftJIS)
		if err != nil {
			retErr = err
			return
		}
		opStr := fmt.Sprintf("Op: %x, argcnt: %d", by, len(info))
		text = append(text, opStr)
		for idx, line := range optxt {
			optxt[idx] = "\t" + line
		}
		text = append(text, optxt...)

		if onlyTxt {
			pureTxt = append(pureTxt, pTxt...)
		}
	}
}

func getInstInfo(exeName string) (instInfo *[256][]byte) {
	fmt.Printf("Opening exe: %s\n", exeName)
	image, baseAddr, err := peHelper.LoadPEImage(exeName)
	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Println("Finding offsets...")
	var off uint32
	off, err = findBytes(image, "8b 2c 85 ?? ?? ?? ?? 85 ed 0f 84 ?? ?? ?? ?? 38 5d 00")
	if err != nil {
		fmt.Println(err)
		return
	}
	mem := memio.Open(image)
	mem.Seek(int64(off+3), 0)
	var startOff uint32
	binary.Read(mem, binary.LittleEndian, &startOff)
	instInfo1 := readInstInfo(&mem, startOff, baseAddr)
	fmt.Println("Op table found.")
	return &instInfo1
}

type nullWriter struct {
}

func (nullWriter) Write(p []byte) (int, error) {
	return len(p), nil
}

func main() {
	exitCode := 1
	defer func() {
		if err := recover(); err != nil {
			fmt.Printf("run time panic: %v", err)
		}
		os.Exit(exitCode)
	}()
	exeName := flag.String("e", "", "specify advhd.exe (optional)")
	inputName := flag.String("i", "", "input ws2 file name")
	outputName := flag.String("o", "", "output txt name")
	onlyTxt := flag.Bool("only-txt", false, "whether parse all info or only txt")
	verbose := flag.Bool("v", false, "print detail information")
	flag.Parse()
	if *inputName == "" || *outputName == "" {
		flag.Usage()
		return
	}
	var nullOutputer nullWriter
	if !*verbose {
		log.SetOutput(nullOutputer)
	}

	fmt.Printf("Parsing %s\n", *inputName)
	var instInfo *[256][]byte
	if *exeName != "" {
		instInfo = getInstInfo(*exeName)
	}
	if instInfo == nil {
		fmt.Println("Using default op table.")
		instInfo = &defaultInstInfo
	}
	gInstInfo = instInfo

	fmt.Println("Parsing ws2...")
	stm, err := ioutil.ReadFile(*inputName)
	if err != nil {
		fmt.Printf("Reading fail. %v\n", err)
		return
	}
	membf := memio.Open(stm)
	var text []string
	var pureTxt []string
	text, pureTxt, err = parseWs2(&membf, instInfo, *onlyTxt)
	if err != nil {
		fmt.Printf("Parsing fail. %v\n", err)
		if !*verbose {
			fmt.Println("you can add -v to view detail info.")
		}
		return
	}

	var finalTxt *[]string
	if *onlyTxt {
		finalTxt = &pureTxt
	} else {
		finalTxt = &text
	}
	fmt.Println("Writing txt...")

	err = ioutil.WriteFile(*outputName,
		codec.Encode(strings.Join(*finalTxt, "\r\n"), codec.UTF8Sig, 0), os.ModePerm)
	if err != nil {
		fmt.Printf("Writing fail. %v\n", err)
		return
	}
	exitCode = 0
}
