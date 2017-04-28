package main

import (
	"encoding/binary"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path"
)

type Header struct {
	Magic     [4]byte
	Entry1Cnt uint32
	Entry2Cnt uint32
	Unk1      uint32
}

type Entry struct {
	FileName    [20]byte
	Flags       uint32
	OffsetIndex uint32
	Length      uint32
}

type FileInfo struct {
	fullName string
	offset   uint32
	length   uint32
}

type Libp struct {
	index         []Entry
	index2        []uint32
	fInfos        []FileInfo
	rootPath      string
	curOffset2    uint32
	curFileOffset uint32
}

func ceilSize(size uint32) uint32 {
	if (size & 1023) == 0 {
		return size
	}
	return (size & ^uint32(1023)) + 1024
}

func (libp *Libp) readDir(path1 string) {
	eles, err := ioutil.ReadDir(path.Join(libp.rootPath, path1))
	if err != nil {
		log.Fatal(err)
	}

	dirs := []string{}
	curIndexOffset := len(libp.index) + len(eles)
	for _, ele := range eles {
		var ent Entry
		copy(ent.FileName[:], []byte(ele.Name()))
		if ele.IsDir() {
			dirs = append(dirs, ele.Name())
			ent.Flags = 0
			newEles, err := ioutil.ReadDir(path.Join(libp.rootPath, path1, ele.Name()))
			if err != nil {
				log.Fatal(err)
			}
			ent.Length = uint32(len(newEles))
			ent.OffsetIndex = uint32(curIndexOffset)
			curIndexOffset += len(newEles)
		} else {
			ent.Flags = 0x10000
			ent.OffsetIndex = libp.curOffset2
			ent.Length = uint32(ele.Size())
			libp.curOffset2++

			var info FileInfo
			info.fullName = path.Join(libp.rootPath, path1, ele.Name())
			info.length = uint32(ele.Size())
			info.offset = libp.curFileOffset
			libp.index2 = append(libp.index2, uint32(info.offset/1024))
			libp.fInfos = append(libp.fInfos, info)
			libp.curFileOffset += ceilSize(uint32(ele.Size()))
		}
		libp.index = append(libp.index, ent)
	}

	for _, dir := range dirs {
		libp.readDir(path.Join(path1, dir))
	}
}

func (libp *Libp) prepare(rootPath string) {
	libp.rootPath = rootPath
	eles, err := ioutil.ReadDir(rootPath)
	if err != nil {
		log.Fatal(err)
	}
	var ent Entry
	ent.OffsetIndex = 1
	ent.Length = uint32(len(eles))
	libp.index = append(libp.index, ent)
	libp.readDir(".")
}

func (libp *Libp) pack(fname string) {
	fs, err := os.Create(fname)
	if err != nil {
		log.Fatal(err)
	}
	defer fs.Close()
	var hdr Header
	copy(hdr.Magic[:], []byte("LIBP"))
	hdr.Entry1Cnt = uint32(len(libp.index))
	hdr.Entry2Cnt = uint32(len(libp.index2))
	baseOff := binary.Size(hdr) + binary.Size(libp.index) + binary.Size(libp.index2)
	baseOff = int(ceilSize(uint32(baseOff)))
	binary.Write(fs, binary.LittleEndian, &hdr)
	binary.Write(fs, binary.LittleEndian, &libp.index)
	binary.Write(fs, binary.LittleEndian, &libp.index2)

	var lastLength uint32
	for _, info := range libp.fInfos {
		bf, err := ioutil.ReadFile(info.fullName)
		if err != nil {
			log.Fatal(err)
		}
		if len(bf) != int(info.length) {
			log.Fatal(fmt.Errorf("length not fit:%s", info.fullName))
		}
		fs.Seek(int64(baseOff+int(info.offset)), 0)
		lastLength = info.length
		fs.Write(bf)
	}
	padSize := ceilSize(lastLength) - lastLength
	pad := make([]byte, padSize)
	fs.Write(pad)
}

func main() {
	var libp Libp
	libp.prepare(`g:\Program Files\sousyu1\data\extra\`)
	libp.pack("a.dat")
	fmt.Printf("over\n")
}
