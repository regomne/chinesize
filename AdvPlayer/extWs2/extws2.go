package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"strconv"
	"strings"

	"io"

	"io/ioutil"
	"os"

	"log"

	"github.com/MJKWoolnough/memio"
	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/pehelper"
)

var defaultInstInfo = [256][]byte{
	[]byte{},
	[]byte{0, 1, 5, 4, 4},
	[]byte{4},
	nil,
	[]byte{10, 8},
	[]byte{},
	[]byte{4},
	[]byte{10, 8},
	[]byte{0},
	[]byte{0, 1, 5},
	[]byte{1, 5},
	[]byte{1, 0},
	[]byte{1, 0, 7, 1},
	[]byte{1, 1, 5},
	[]byte{1, 1, 0},
	[]byte{0},
	nil,
	[]byte{6, 8, 5},
	[]byte{6, 8, 0, 10, 8},
	[]byte{},
	[]byte{4, 6, 8, 6, 8, 0},
	[]byte{6, 8, 0},
	[]byte{0, 0},
	[]byte{},
	[]byte{0, 6, 8},
	[]byte{},
	[]byte{6, 8},
	[]byte{0},
	[]byte{6, 8, 6, 8, 1, 0},
	[]byte{1},
	[]byte{6, 8, 10, 8, 5, 5, 1, 1, 0, 5},
	[]byte{6, 8, 5},
	[]byte{6, 8, 5, 1},
	[]byte{6, 8, 1, 1, 1},
	[]byte{6, 8, 0},
	nil,
	nil,
	nil,
	nil,
	nil,
	[]byte{6, 8, 10, 8, 5, 5, 1, 1, 0, 1, 1, 0, 5},
	[]byte{6, 8, 5},
	[]byte{6, 8, 5, 1},
	[]byte{6, 8},
	[]byte{6, 8},
	[]byte{6, 8, 0},
	[]byte{},
	[]byte{6, 8, 1, 5},
	nil,
	nil,
	[]byte{10, 8},
	[]byte{6, 8, 10, 8, 0, 0},
	[]byte{6, 8, 10, 8, 0, 0},
	[]byte{6, 8, 10, 8, 0, 0, 0},
	[]byte{6, 8, 5, 5, 5, 5, 5, 5, 5, 0, 0},
	[]byte{6, 8},
	[]byte{6, 8, 0},
	[]byte{6, 8, 0, 0, 7, 1},
	[]byte{6, 8, 0, 0},
	[]byte{6, 8, 6, 8, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5},
	[]byte{6, 8},
	[]byte{1},
	[]byte{},
	[]byte{7, 6},
	[]byte{6, 8, 10, 8, 0},
	[]byte{6, 8, 0},
	[]byte{6, 8, 1},
	[]byte{6, 8},
	[]byte{6, 8, 6, 8, 0},
	[]byte{6, 8, 1, 5, 5, 5, 5},
	[]byte{6, 8, 1, 0, 5, 5, 5, 5},
	[]byte{6, 8, 6, 8, 1, 0, 0, 5, 5, 5, 5, 5, 1, 5},
	[]byte{6, 8, 6, 8, 1, 0, 0, 10, 8},
	[]byte{6, 8, 6, 8, 10, 8},
	[]byte{6, 8, 6, 8},
	[]byte{6, 8, 1, 1, 5, 5, 5, 5},
	[]byte{6, 8, 1, 1, 0, 5, 5, 5, 5},
	[]byte{6, 8, 6, 8, 1, 1, 0, 0, 5, 5, 5, 5, 5, 1, 5},
	[]byte{6, 8, 6, 8, 1, 1, 0, 0, 10, 8},
	[]byte{6, 8, 6, 8, 1, 10, 8},
	[]byte{6, 8, 6, 8, 1},
	[]byte{6, 8, 6, 8, 1, 5, 0},
	[]byte{6, 8, 6, 8, 5, 1, 5, 0, 10, 8},
	[]byte{6, 8, 6, 8},
	[]byte{6, 8, 6, 8, 10, 8},
	[]byte{6, 8, 6, 8},
	[]byte{6, 8, 0, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 5, 5, 5, 5, 0, 1, 6, 8, 1, 6, 8, 10, 8, 5},
	[]byte{6, 8, 1},
	[]byte{6, 8, 6, 8},
	[]byte{6, 8, 6, 8, 1},
	[]byte{6, 8, 7, 1},
	[]byte{6, 8, 1, 0},
	[]byte{6, 8},
	[]byte{6, 8, 6, 8, 0},
	[]byte{6, 8, 5, 5},
	[]byte{10, 8},
	[]byte{1, 1, 1, 1},
	[]byte{0, 5, 5, 5, 5},
	[]byte{6, 8},
	[]byte{6, 8, 0},
	[]byte{0},
	[]byte{1, 0, 5, 5, 0, 10, 8},
	[]byte{10, 8},
	[]byte{0, 0, 1, 5, 5, 5, 5, 5, 0},
	[]byte{0},
	[]byte{6, 8, 0, 0, 5, 5, 5, 5, 5, 1, 5},
	[]byte{6, 8, 1, 0, 0, 10, 8},
	[]byte{6, 8, 6, 8},
	nil,
	nil,
	[]byte{9, 8, 9, 8},
	[]byte{9, 8},
	[]byte{9, 8, 1},
	[]byte{},
	[]byte{9, 8, 1, 1, 9, 8},
	[]byte{9, 8, 9, 8, 1},
	[]byte{9, 8, 9, 8},
	[]byte{9, 8, 6, 8},
	nil,
	nil,
	[]byte{6, 8, 10, 8, 0, 0, 0},
	[]byte{6, 8, 6, 8, 5},
	[]byte{6, 8, 10, 8, 5, 0, 0, 10, 8},
	[]byte{6, 8, 10, 8},
	[]byte{6, 8, 6, 8, 5},
	[]byte{6, 8, 5},
	[]byte{6, 8},
	[]byte{6, 8, 5, 5, 5, 5, 5},
	[]byte{6, 8},
	[]byte{6, 8, 0, 10, 8, 5, 5, 0},
	[]byte{6, 8, 10, 8, 5},
	[]byte{6, 8, 6, 8, 5, 5},
	[]byte{6, 8, 6, 8, 6, 8, 5},
	[]byte{6, 8, 6, 8, 0, 5},
	[]byte{6, 8, 5, 5, 5},
	nil,
	nil,
	nil,
	nil,
	nil,
	[]byte{6, 8, 10, 8, 6, 8, 0, 0},
	[]byte{4, 6, 8, 6, 8, 0, 0, 10, 8},
	[]byte{6, 8, 6, 8, 0, 0, 10, 8},
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	[]byte{},
	[]byte{6, 8, 6, 8, 1, 1, 1},
	[]byte{6, 8, 6, 8},
	[]byte{6, 8, 0, 0},
	[]byte{},
	[]byte{6, 8, 6, 8, 6, 8, 6, 8, 6, 8, 5, 0},
	[]byte{0},
	[]byte{6, 8, 6, 8, 5},
	[]byte{6, 8, 1},
	[]byte{6, 8, 1},
	[]byte{6, 8},
	[]byte{6, 8},
	[]byte{10, 8, 1, 1},
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	[]byte{0},
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	[]byte{},
	[]byte{0, 10, 8},
	[]byte{},
	[]byte{0},
	[]byte{1},
	[]byte{},
	[]byte{6, 8},
	nil,
}

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

func parseInst(buf *memio.ReadMem, op byte, info []byte, strCodec int) (text []string, retErr error) {
	text = make([]string, 0, len(info)+1)
	text = append(text, fmt.Sprintf("Op %x, argcnt: %d", op, len(info)))
	for _, op := range info {
		switch op {
		case tByte:
			b, _ := buf.ReadByte()
			text = append(text, fmt.Sprintf("\tByte: %d", b))
		case tShort:
			var v int16
			binary.Read(buf, binary.LittleEndian, &v)
			text = append(text, fmt.Sprintf("\tShort: %d", v))
		case tWord:
			var v uint16
			binary.Read(buf, binary.LittleEndian, &v)
			text = append(text, fmt.Sprintf("\tWord: %d", v))
		case tLong:
			var v int32
			binary.Read(buf, binary.LittleEndian, &v)
			text = append(text, fmt.Sprintf("\tLong: %d", v))
		case tDword:
			var v uint32
			binary.Read(buf, binary.LittleEndian, &v)
			text = append(text, fmt.Sprintf("\tDword: %d", v))
		case tFloat:
			var v float32
			binary.Read(buf, binary.LittleEndian, &v)
			text = append(text, fmt.Sprintf("\tFloat: %f", v))
		case tStr:
			s := readUntil(*buf, 0)
			buf.UnreadByte()
			text = append(text, fmt.Sprintf("\tStr: %s", codec.Decode(s, strCodec)))
		case tStrDefine:
			s := readUntil(*buf, 0)
			buf.UnreadByte()
			text = append(text, fmt.Sprintf("\tStrDef: %s", codec.Decode(s, strCodec)))
		case tStrFileName:
			s := readUntil(*buf, 0)
			buf.UnreadByte()
			text = append(text, fmt.Sprintf("\tStrFile: %s", codec.Decode(s, strCodec)))
		case tVariable:
			text = append(text, "\tVar")
		case tDelimiter:
			buf.ReadByte()
		default:
			panic(fmt.Errorf("unk op code"))
		}
	}
	return
}

func parseWs2(buf *memio.ReadMem, instInfo *[256][]byte) (text []string, retErr error) {
	text = []string{}
	for {
		by, err := buf.ReadByte()
		if err != nil {
			if err.Error() == "EOF" {
				return
			}
			retErr = err
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
		optxt, err = parseInst(buf, by, info, codec.C932)
		if err != nil {
			retErr = err
			return
		}
		text = append(text, optxt...)
	}
}

const ws2Name = `f:\chinesize\AdvPlayer\extArc\RioDec\chi_01.ws2`

func main() {
	image, baseAddr, err := peHelper.LoadPEImage(`d:\galgame\想いを捧げる乙女のメロディー\AdvHD2.exe`)
	if err != nil {
		fmt.Println(err)
		return
	}
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
	instInfo := readInstInfo(&mem, startOff, baseAddr)

	stm, _ := ioutil.ReadFile(ws2Name)
	membf := memio.Open(stm)
	var text []string
	text, err = parseWs2(&membf, &instInfo)
	if err != nil {
		fmt.Println(err)
		return
	}

	ioutil.WriteFile("txt.txt", []byte(strings.Join(text, "\r\n")), os.ModePerm)

}
