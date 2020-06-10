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

	"hash/crc32"

	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/peHelper"
)

var gInstInfo *[256][]byte
var gNamePrefix *string

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

func findBytes(buf []byte, pattern string) (int, error) {
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
			return i, nil
		}
	}
	return 0, fmt.Errorf("can't find pattern")
}

func readInstInfo(mem *bytes.Reader, startOff uint32, base uint32) (instInfo [256][]byte) {
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

func getOp(buf *bytes.Reader, op byte, opNext byte, strCodec int) (
	val interface{}, text string, retErr error) {
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
		s := readUntil(buf, 0)
		buf.UnreadByte()
		ns := codec.Decode(s, strCodec)
		text = fmt.Sprintf("Str: %s", ns)
		val = ns
	case tStrDefine:
		s := readUntil(buf, 0)
		buf.UnreadByte()
		ns := codec.Decode(s, strCodec)
		text = fmt.Sprintf("StrDef: %s", ns)
		val = ns
	case tStrFileName:
		s := readUntil(buf, 0)
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

func getSelectItem(buf *bytes.Reader, strCodec int) (
	text string, sel string, selIdx int64, retErr error) {
	var (
		w1 uint16
		b1 uint8
		w2 uint16
	)
	binary.Read(buf, binary.LittleEndian, &w1)
	selIdx, _ = buf.Seek(0, 1)
	s := readUntil(buf, 0)
	sel = codec.Decode(s, strCodec)
	binary.Read(buf, binary.LittleEndian, &b1)
	binary.Read(buf, binary.LittleEndian, &w2)
	op, _ := buf.ReadByte()
	ls, _, _, err := parseInst(buf, op, gInstInfo[op], strCodec)
	if err != nil {
		retErr = err
		return
	}
	text = fmt.Sprintf("w1:%d, b1:%d, w2:%d, lines:%v, sel:%s",
		w1, b1, w2, ls, sel)
	return
}

func parseInst(buf *bytes.Reader, oriOp byte, info []byte, strCodec int) (
	text []string, pureTxt []string, txtIdx []int64, retErr error) {
	text = make([]string, 0, len(info))
	pureTxt = make([]string, 0, len(info))
	txtIdx = make([]int64, 0, len(info))
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
			line, sel, selIdx, err := getSelectItem(buf, strCodec)
			if err != nil {
				retErr = err
				return
			}
			text = append(text, fmt.Sprintf("%d: %s", i, line))
			pureTxt = append(pureTxt, "Sel:"+sel)
			txtIdx = append(txtIdx, selIdx)
		}
		return
	}
	indexes := make([]int64, 0, len(info))
	for i := 0; i < len(info); i++ {
		op := info[i]
		if op == tVariable {
			pos, _ := buf.Seek(0, 1)
			val, line, err := getOp(buf, op, info[i+1], strCodec)
			if err != nil {
				retErr = err
				return
			}
			i++
			text = append(text, line)
			vals = append(vals, val)
			indexes = append(indexes, pos)
		} else {
			pos, _ := buf.Seek(0, 1)
			val, line, err := getOp(buf, op, 255, strCodec)
			if err != nil {
				retErr = err
				return
			}
			if line != "" {
				text = append(text, line)
				vals = append(vals, val)
				indexes = append(indexes, pos)
			}
		}
	}

	if oriOp == 0x15 {
		s, _ := vals[0].(string)
		if len(s) != 0 {
			if strings.Index(s, *gNamePrefix) == 0 {
				s = s[3:]
			}
			pureTxt = append(pureTxt, s)
			txtIdx = append(txtIdx, indexes[0])
		}
	} else if oriOp == 0x14 {
		s, _ := vals[2].(string)
		if strings.LastIndex(s, "%K%P") == len(s)-4 {
			s = s[:len(s)-4]
		}
		pureTxt = append(pureTxt, s)
		txtIdx = append(txtIdx, indexes[2])
	}
	return
}

func parseWs2(buf *bytes.Reader, instInfo *[256][]byte, onlyTxt bool, cp int) (
	text []string, pureTxt []string, txtIdx []int64, checkSum uint32, retErr error) {
	text = []string{}
	pureTxt = []string{}
	txtIdx = []int64{}
	for {
		by, err := buf.ReadByte()
		if err != nil {
			retErr = err
			return
		}
		if by == 255 {
			pos, _ := buf.Seek(0, 1)
			if pos > 0x1000 {
				pos = 0x1000
			}
			sli := make([]byte, pos)
			buf.ReadAt(sli, 0)
			checkSum = crc32.ChecksumIEEE(sli)
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
		var subTxtIdx []int64
		optxt, pTxt, subTxtIdx, err = parseInst(buf, by, info, cp)
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
			txtIdx = append(txtIdx, subTxtIdx...)
		}
	}
}

func searchInstInfo(image []byte, pat string, off int) (fileOff int, err error) {
	if pat != "" {
		fmt.Println("Using user-define op table pattern...")
		fileOff, err = findBytes(image, pat)
		if err == nil {
			fileOff += off
		}
		return
	}
	// Steps:
	// 1 find pattern: 01 05 04 04 04, get addr A
	// 2 xref the addr of A, get B
	// 3 xref B, get the InstInfo addr
	Pats := []string{
		// AdvHD 1.5
		"8b 2c 85 ?? ?? ?? ?? 85 ed 0f 84 ?? ?? ?? ?? 38 5d 00",
		// AdvHD 1.7
		"0f b6 45 ?? 33 ff 89 4d ?? 8b 04 85 ?? ?? ?? ?? 89 45",
		// AdvHD 1.9
		"C7 45 ?? 00 00 00 00 8B 0C 85 ?? ?? ?? ?? 89 4D",
	}
	Offs := []int{
		3,
		12,
		10,
	}
	for i := 0; i < len(Pats); i++ {
		fileOff, err = findBytes(image, Pats[i])
		if err == nil {
			fileOff += Offs[i]
			return
		}
	}
	return
}

func getInstInfo(exeName string, pat string, off int) (instInfo *[256][]byte) {
	fmt.Printf("Opening exe: %s\n", exeName)
	image, baseAddr, err := peHelper.LoadPEImage(exeName)
	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Println("Finding offsets...")
	fileOff, err := searchInstInfo(image, pat, off)
	if err != nil {
		fmt.Println(err)
		return
	}
	mem := bytes.NewReader(image)
	mem.Seek(int64(fileOff), 0)
	var startOff uint32
	binary.Read(mem, binary.LittleEndian, &startOff)
	instInfo1 := readInstInfo(mem, startOff, baseAddr)
	fmt.Println("Op table found.")
	return &instInfo1
}

type nullWriter struct {
}

func (nullWriter) Write(p []byte) (int, error) {
	return len(p), nil
}

func writeIndexToFile(fname string, txtIdx []int64, crc uint32) error {
	bf := bytes.NewBuffer([]byte{})
	binary.Write(bf, binary.LittleEndian, &crc)
	for _, idx := range txtIdx {
		idx32 := uint32(idx)
		binary.Write(bf, binary.LittleEndian, &idx32)
	}
	return ioutil.WriteFile(fname, bf.Bytes(), os.ModePerm)
}

func getCodePageFromString(s string) (cp int, err error) {
	switch s {
	case "932":
		cp = codec.C932
	case "936":
		cp = codec.C936
	default:
		err = fmt.Errorf("unknown code page %s", s)
	}
	return
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
	indexName := flag.String("index-file", "", "file name for pure txt index and checksum (for chinesize)")
	textCodePage := flag.String("cp", "932", "specific code page of text in ws2")
	gNamePrefix = flag.String("name-prefix", "%LC", "set name prefix when only txt")
	verbose := flag.Bool("v", false, "print detail information")
	pattern := flag.String("exe-pat", "", "specify table pattern for advhd.exe")
	patternOffset := flag.Int("exe-pat-off", 0, "specify offset of pattern for advhd.exe")
	flag.Parse()
	if *inputName == "" || *outputName == "" {
		flag.Usage()
		return
	}
	cp, cpErr := getCodePageFromString(*textCodePage)
	if cpErr != nil {
		fmt.Println(cpErr)
		return
	}
	var nullOutputer nullWriter
	if !*verbose {
		log.SetOutput(nullOutputer)
	}

	fmt.Printf("Parsing %s\n", *inputName)
	var instInfo *[256][]byte
	if *exeName != "" {
		instInfo = getInstInfo(*exeName, *pattern, *patternOffset)
	}
	if instInfo == nil {
		fmt.Println("Using default op table. You can find the table by\nsearching exe with one of defaultInst.")
		instInfo = &defaultInstInfo
	}
	gInstInfo = instInfo

	fmt.Println("Parsing ws2...")
	stm, err := ioutil.ReadFile(*inputName)
	if err != nil {
		fmt.Printf("Reading fail. %v\n", err)
		return
	}
	membf := bytes.NewReader(stm)
	var text []string
	var pureTxt []string
	var txtIdx []int64
	var crc uint32
	text, pureTxt, txtIdx, crc, err = parseWs2(membf, instInfo, *onlyTxt, cp)
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
		fmt.Printf("Writing txt fail. %v\n", err)
		return
	}
	if *onlyTxt && *indexName != "" {
		if len(pureTxt) != len(txtIdx) {
			panic(fmt.Errorf("code error"))
		}
		fmt.Println("Writing idx...")
		err = writeIndexToFile(*indexName, txtIdx, crc)
		if err != nil {
			fmt.Printf("Writing idx fail. %v\n", err)
			return
		}
	}
	exitCode = 0
}
