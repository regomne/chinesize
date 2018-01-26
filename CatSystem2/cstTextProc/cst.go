package main

import (
	"bytes"
	"compress/zlib"
	"encoding/binary"
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"regexp"
	"strings"

	"github.com/regomne/eutil/codec"
	"github.com/regomne/eutil/textFile"
)

type CstHeader struct {
	Magic       [8]byte
	ComprSize   uint32
	UncomprSize uint32
}

type CstHeader2 struct {
	ContentSize   uint32
	CmdCount      uint32
	CmdTableSize  uint32
	ResourceStart uint32
}

type CstInfo struct {
	header       CstHeader2
	cmdTable     []byte
	rsrcOffTable []uint32
	rscList      [][]byte
}

func readAndDecomprCst(fname string) (stream []byte, err error) {
	fs, err := os.Open(fname)
	if err != nil {
		return
	}
	defer fs.Close()

	var header CstHeader
	binary.Read(fs, binary.LittleEndian, &header)
	if !bytes.Equal(header.Magic[:], []byte("CatScene")) {
		err = fmt.Errorf("not a cst file")
		return
	}

	rdr, err := zlib.NewReader(fs)
	if err != nil {
		return
	}
	stream, err = ioutil.ReadAll(rdr)
	if err != nil {
		return
	}
	if len(stream) != int(header.UncomprSize) {
		err = fmt.Errorf("error decompressed size, should be:%d, real:%d", header.UncomprSize, len(stream))
		return
	}
	return
}

func compressAndWriteCst(fname string, content []byte) (err error) {
	var header CstHeader
	copy(header.Magic[:], []byte("CatScene"))
	fs, err := os.Create(fname)
	if err != nil {
		return
	}
	defer fs.Close()
	fs.Seek(0x10, 0)
	zlibWorker := zlib.NewWriter(fs)
	_, err = zlibWorker.Write(content)
	if err != nil {
		return
	}
	zlibWorker.Close()
	curOff, _ := fs.Seek(0, 1)
	header.ComprSize = uint32(curOff - 0x10)
	header.UncomprSize = uint32(len(content))
	fs.Seek(0, 0)
	binary.Write(fs, binary.LittleEndian, &header)
	return
}

func extCstText(stm []byte) (txtList [][]byte, cstInfo CstInfo, err error) {
	reader := bytes.NewReader(stm)
	var header CstHeader2
	binary.Read(reader, binary.LittleEndian, &header)
	if len(stm) != int(header.ContentSize)+binary.Size(header) {
		err = fmt.Errorf("content size error")
		return
	}
	cstInfo.header = header
	cstInfo.cmdTable = make([]byte, header.CmdTableSize)
	reader.Read(cstInfo.cmdTable)

	rscCnt := (header.ResourceStart - header.CmdTableSize) / 4
	rscOffTable := make([]uint32, rscCnt)
	binary.Read(reader, binary.LittleEndian, rscOffTable)
	cstInfo.rsrcOffTable = rscOffTable

	rsc := make([]byte, header.ContentSize-header.ResourceStart)
	reader.Read(rsc)
	txtList = make([][]byte, 0, rscCnt)
	cstInfo.rscList = make([][]byte, rscCnt)
	for idx, off := range rscOffTable {
		ends := bytes.IndexByte(rsc[off+2:], 0)
		if ends == -1 {
			err = fmt.Errorf("unexpected end")
			return
		}
		element := rsc[off : int(off)+2+ends]
		cstInfo.rscList[idx] = element
		line := rsc[off+2 : int(off)+2+ends]
		if isNeedExtract(rsc[off:]) {
			txtList = append(txtList, line)
		}

		sli := rsc[off : off+3]
		if sli[1] != 2 && sli[1] != 0x20 && sli[1] != 0x30 &&
			sli[1] != 0x21 {
			fmt.Printf("off:0x%x\n", off)
		}
	}
	return
}

func isAllHalf(s []byte) bool {
	for _, c := range s {
		if c >= 0x80 {
			return false
		}
	}
	return true
}

func isNeedExtract(line []byte) bool {
	c := line[1]
	if c == 0x20 || c == 0x21 {
		return true
	}
	isSel, err := regexp.Match(`^\d \w.*`, line[2:])
	if err != nil {
		return false
	}
	return isSel
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

func packTextToCst(txtList [][]byte, cstInfo *CstInfo) (newStm []byte, err error) {
	txtIdx := 0
	newRsc := make([]byte, 0, cstInfo.header.ContentSize)
	temp := make([]byte, 2)
	for idx, ele := range cstInfo.rscList {
		cstInfo.rsrcOffTable[idx] = uint32(len(newRsc))
		if isNeedExtract(ele) {
			copy(temp, ele[0:2])
			cstInfo.rscList[idx] = append(temp[0:2], txtList[txtIdx]...)
			txtIdx++
		}
		newRsc = append(newRsc, cstInfo.rscList[idx]...)
		newRsc = append(newRsc, 0)
	}
	var stm bytes.Buffer
	cstInfo.header.ContentSize = cstInfo.header.ResourceStart + uint32(len(newRsc))
	binary.Write(&stm, binary.LittleEndian, &cstInfo.header)
	stm.Write(cstInfo.cmdTable)
	binary.Write(&stm, binary.LittleEndian, cstInfo.rsrcOffTable)
	stm.Write(newRsc)
	newStm = stm.Bytes()
	return
}

func packCst(cstName, txtName, newCstName string, cp int) bool {
	fmt.Println("decompressing original file...")
	stm, err := readAndDecomprCst(cstName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	fmt.Println("extracting...")
	oriTxts, cstInfo, err := extCstText(stm)
	if err != nil {
		fmt.Println(err)
		return false
	}
	fmt.Println("reading txtfile...")
	lines, err := textFile.ReadWin32TxtToLines(txtName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	// for lines[len(lines)-1] == "" {
	// 	lines = lines[:len(lines)-1]
	// }
	if len(lines) != len(oriTxts) {
		fmt.Printf("txt line not fit, should be:%d, real:%d\n", len(oriTxts), len(lines))
		return false
	}
	txtList := encodeLines(lines, cp)
	fmt.Println("packing cst...")
	newStm, err := packTextToCst(txtList, &cstInfo)
	if err != nil {
		fmt.Println(err)
		return false
	}
	fmt.Println("compressing file...")
	err = compressAndWriteCst(newCstName, newStm)
	if err != nil {
		fmt.Println(err)
		return false
	}
	fmt.Println("complete.")
	return true
}

func decodeLines(raw [][]byte, cp int) (strs []string) {
	strs = make([]string, len(raw))
	for i, byteStr := range raw {
		strs[i] = codec.Decode(byteStr, cp)
	}
	return
}

func encodeLines(strs []string, cp int) (bs [][]byte) {
	bs = make([][]byte, len(strs))
	for i, str := range strs {
		bs[i] = codec.Encode(str, cp, codec.Replace)
	}
	return
}

func writeText(fname string, list []string) (err error) {
	return ioutil.WriteFile(fname,
		codec.Encode(strings.Join(list, "\r\n"), codec.UTF8Sig, codec.ReplaceHTML), os.ModePerm)
}

func extractCst(cstName, txtName string, codePage int) bool {
	fmt.Println("decompressing file...")
	stm, err := readAndDecomprCst(cstName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	fmt.Println("extracting text...")
	txtList, _, err := extCstText(stm)
	if err != nil {
		fmt.Println(err)
		return false
	}
	fmt.Println("writing text...")
	strList := decodeLines(txtList, codePage)
	err = writeText(txtName, strList)
	if err != nil {
		fmt.Println(err)
		return false
	}
	fmt.Println("complete.")
	return true
}

func main() {
	exitCode := 0
	defer func() {
		os.Exit(exitCode)
	}()

	isExt := flag.Bool("e", false, "extract cst to txt")
	isPack := flag.Bool("p", false, "pack txt to a cst")
	cstName := flag.String("cst", "", "specify the cst name")
	txtName := flag.String("txt", "", "specify the txt name(used in pack)")
	outputName := flag.String("o", "", "output file name(txt or cst)")
	codePage := flag.String("cp", "932", "the code page used in txt")

	flag.Parse()
	if (*isExt && *isPack) || (!*isExt && !*isPack) ||
		(*isExt && (*cstName == "" || *outputName == "")) ||
		(*isPack && (*cstName == "" || *txtName == "" || *outputName == "")) {
		flag.Usage()
		return
	}

	cp, err := getCodePageFromString(*codePage)
	if err != nil {
		fmt.Println(err)
		return
	}

	if *isExt {
		if !extractCst(*cstName, *outputName, cp) {
			exitCode = 1
		}
	} else if *isPack {
		if !packCst(*cstName, *txtName, *outputName, cp) {
			exitCode = 1
		}
	}

	return
}
