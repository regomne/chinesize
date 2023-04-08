package main

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"github.com/regomne/eutil/codec"
	"io"
	"os"
	"strings"
)

type YbnHeader struct {
	Magic   [4]byte
	Version uint32
}

func readFileToString(fileName string, codePage int) (s string, err error) {
	file, err := os.Open(fileName)
	if err != nil {
		return
	}
	defer file.Close()

	stats, statsErr := file.Stat()
	if statsErr != nil {
		err = statsErr
		return
	}

	var size = stats.Size()
	fileBytes := make([]byte, size)
	bufr := bufio.NewReader(file)
	_, readErr := bufr.Read(fileBytes)
	if readErr != nil {
		err = readErr
		return
	}
	s = codec.Decode(fileBytes, codePage)
	return
}

func readFileToLines(fileName string, codePage int) (s []string, err error) {
	txt, readErr := readFileToString(fileName, codePage)
	if readErr != nil {
		return nil, readErr
	}
	s = strings.Split(txt, "\n")
	for i := range s {
		s[i] = strings.Trim(s[i], "\r\n")
	}
	return
}

func readAnsiStr(r io.Reader, codePage int) string {
	var bf bytes.Buffer
	var b byte
	binary.Read(r, binary.LittleEndian, &b)
	for b != 0 {
		bf.WriteByte(b)
		binary.Read(r, binary.LittleEndian, &b)
	}
	buffer := bf.Bytes()
	if len(buffer) == 0 {
		return ""
	}
	return codec.Decode(buffer, codePage)
}

type BufferWriter struct {
	Buffer *bytes.Buffer
}

func (buff *BufferWriter) Write(b []byte) (n int, err error) {
	n, err = buff.Buffer.Write(b)
	return
}
