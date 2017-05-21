package main

import (
	"encoding/binary"
	"io"
	"io/ioutil"
	"log"
	"os"
)

var gRandBytes []byte

type Entry struct {
	Name   [51]byte
	Offset uint32
	Length uint32
}

func packList(files []string, newpack string) {
	fs, err := os.Create(newpack)
	if err != nil {
		log.Fatal(err)
	}
	defer fs.Close()
	fileCnt := uint32(len(files))
	binary.Write(fs, binary.LittleEndian, &fileCnt)
	entries := make([]Entry, len(files))

	curOff := 4 + binary.Size(entries)
	for i, fileName := range files {
		copy(entries[i].Name[:], []byte(fileName))
		entries[i].Offset = uint32(curOff)
		file, err := ioutil.ReadFile(fileName)
		if err != nil {
			log.Fatal(err)
		}
		entries[i].Length = uint32(len(file))
		fs.Seek(int64(curOff), 0)
		fs.Write(file)
		curOff += len(file)
	}
	fs.Seek(4, 0)
	binary.Write(fs, binary.LittleEndian, &entries)
}

func encFile(fname string) {
	fs, err := os.OpenFile(fname, os.O_RDWR, os.ModePerm)
	if err != nil {
		log.Fatal(err)
	}
	defer fs.Close()
	fSize, _ := fs.Seek(0, 2)
	fs.Seek(0, 0)
	bf := make([]byte, 1024)
	for fSize > 0 {
		readSize, err := fs.Read(bf)
		if err != nil && err != io.EOF {
			log.Fatal(err)
		}
		for i, b := range bf {
			bf[i] = b ^ gRandBytes[i]
		}
		fs.Seek(int64(-readSize), 1)
		fs.Write(bf[0:readSize])
		fSize -= int64(readSize)
	}
}

func cvtToList(prefix string, fInfos []os.FileInfo) (files []string) {
	files = make([]string, len(fInfos))
	for i, info := range fInfos {
		files[i] = prefix + info.Name()
	}
	return
}

func main() {
	//gRandBytes = make([]byte, 1024)
	//rand.Read(gRandBytes)
	//ioutil.WriteFile("key.key", gRandBytes, os.ModePerm)
	gRandBytes, _ = ioutil.ReadFile("key.key")

	f1, _ := ioutil.ReadDir(`txt\`)
	files1 := cvtToList("txt\\", f1)
	// f2, _ := ioutil.ReadDir(`idx\`)
	// files2 := cvtToList("idx\\", f2)
	// files1 = append(files1, files2...)
	packList(files1, "cnpack")
	encFile("cnpack")
}
