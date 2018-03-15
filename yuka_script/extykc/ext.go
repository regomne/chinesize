package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"path/filepath"
)

type YkcHeader struct {
	Magic       [8]uint8
	HeaderSize  uint32
	Resv        uint32
	IndexOffset uint32
	IndexSize   uint32
}

type YkcEntry struct {
	NameOffset uint32
	NameLength uint32
	Offset     uint32
	Size       uint32
	Resv       uint32
}

func extYkc(fname, outputFolder string) bool {
	fs, err := os.Open(fname)
	if err != nil {
		fmt.Println("Can't open file, err:", err)
		return false
	}
	defer fs.Close()
	var header YkcHeader
	binary.Read(fs, binary.LittleEndian, &header)
	if bytes.Compare(header.Magic[:], []byte("YKC001\x00\x00")) != 0 {
		fmt.Println("file format err!")
		return false
	}

	fs.Seek(int64(header.IndexOffset), 0)
	return false
}

func packYkc(fname, inFolder string) bool {
	entries := make([]YkcEntry, 0, 100)
	names := make([]uint8, 0, 100)
	fs, err := os.Create(fname)
	if err != nil {
		fmt.Println("Can't create file, err:", err)
		return false
	}
	defer fs.Close()
	fs.Seek(0x18, 0)

	filepath.Walk(inFolder, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			fmt.Println("walk dir error, err:", err)
			return err
		}
		if info.IsDir() {
			return nil
		}
		relPath, _ := filepath.Rel(inFolder, path)
		newf, err := os.Open(path)
		if err != nil {
			fmt.Printf("open file %s error\n", path)
			return err
		}
		defer newf.Close()
		io.Copy(fs, newf)
		return nil
	})
}

func main() {

}
