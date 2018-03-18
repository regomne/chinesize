package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"github.com/regomne/eutil/codec"
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

type fileEntry struct {
	path string
	size int64
}

func packYkc(fname, inFolder string) bool {
	entries := make([]YkcEntry, 0, 100)
	names := make([][]uint8, 0, 100)
	fs, err := os.Create(fname)
	if err != nil {
		fmt.Println("Can't create file, err:", err)
		return false
	}
	defer fs.Close()
	fs.Seek(0x18, 0)

	curNameOffset := 0
	curFileOffset := 0x18
	filelists := make([]fileEntry, 0, 100)
	filepath.Walk(inFolder, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			fmt.Println("walk dir error, err:", err)
			return err
		}
		if info.IsDir() {
			return nil
		}
		filelists = append(filelists, fileEntry{path, info.Size()})
		return nil
	})

	sort.Slice(filelists, func(i, j int) bool {
		p1 := strings.ToLower(filelists[i].path)
		p2 := strings.ToLower(filelists[j].path)
		s1 := codec.Encode(p1, codec.C932, codec.Replace)
		s2 := codec.Encode(p2, codec.C932, codec.Replace)
		return bytes.Compare(s1, s2) < 0
	})

	for _, fileInfo := range filelists {
		path := fileInfo.path
		relPath, _ := filepath.Rel(inFolder, path)
		relPath += "\x00"
		newf, err := os.Open(path)
		if err != nil {
			fmt.Printf("open file %s error\n", path)
			return false
		}
		fmt.Printf("packing %s ...\n", relPath)
		defer newf.Close()
		io.Copy(fs, newf)
		encodedPath := codec.Encode(relPath, codec.C932, codec.Replace)
		names = append(names, encodedPath)
		var ent YkcEntry
		ent.NameLength = uint32(len(encodedPath))
		ent.NameOffset = uint32(curNameOffset)
		curNameOffset += int(ent.NameLength)
		ent.Offset = uint32(curFileOffset)
		ent.Size = uint32(fileInfo.size)
		curFileOffset += int(ent.Size)
		entries = append(entries, ent)
	}

	fmt.Printf("writing index...\n")
	for _, name := range names {
		fs.Write(name)
	}
	curOff, _ := fs.Seek(0, 1)
	var header YkcHeader
	header.HeaderSize = 0x18
	header.IndexOffset = uint32(curOff)
	header.IndexSize = uint32(len(entries) * 20)
	for idx := range entries {
		entries[idx].NameOffset += uint32(curFileOffset)
	}
	binary.Write(fs, binary.LittleEndian, &entries)
	fs.Seek(0, 0)
	copy(header.Magic[:], []byte("YKC001\x00\x00"))
	binary.Write(fs, binary.LittleEndian, &header)
	fmt.Printf("complete.")
	return true
}

func main() {
	exitCode := 0
	defer func() {
		os.Exit(exitCode)
	}()
	if len(os.Args) != 3 {
		fmt.Printf("Usage: %s <input_dir> <output.ykc>\n", os.Args[0])
		return
	}
	packYkc(os.Args[2], os.Args[1])
}
