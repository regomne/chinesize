package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io/ioutil"
	"os"
	"path"

	"github.com/regomne/eutil/codec"
)

type fileEntry struct {
	name        string
	nameEncoded []byte
	size        int64
}

type fileEntrySerialized struct {
	CompSize uint32
	UncSize  uint32
	Offset   uint32
}

func encryptName(s []byte) (ns []byte) {
	ns = make([]byte, len(s))
	k := uint8(len(s))
	for i := 0; i < len(s); i++ {
		ns[i] = s[i] - k
		k--
	}
	return
}

func lzssPack(ori []byte) []byte {
	var out bytes.Buffer
	plen := len(ori) & (^7)
	for i := 0; i < plen; i += 8 {
		out.WriteByte(0xff)
		out.Write(ori[i : i+8])
	}
	llen := len(ori) - plen
	if llen > 0 {
		out.WriteByte(uint8((1 << uint8(llen)) - 1))
		out.Write(ori[plen:])
	}
	return out.Bytes()
}

func packArc(arcname, dirname string, fileNameCp int) bool {
	dirInfo, err := ioutil.ReadDir(dirname)
	if err != nil {
		fmt.Println("open dir fail:", err)
		return false
	}
	fileList := make([]fileEntry, 0, len(dirInfo))
	indexSize := int32(0)
	for _, info := range dirInfo {
		if info.IsDir() {
			continue
		}
		ent := fileEntry{}
		ent.name = info.Name()
		ent.size = info.Size()
		ent.nameEncoded = codec.Encode(ent.name, fileNameCp, codec.Replace)
		ent.nameEncoded = encryptName(ent.nameEncoded)
		fileList = append(fileList, ent)

		indexSize += int32(len(ent.nameEncoded) + 1 + 12)
	}

	fs, err := os.Create(arcname)
	if err != nil {
		fmt.Println("create arc fail:", err)
		return false
	}
	defer fs.Close()

	binary.Write(fs, binary.LittleEndian, indexSize)
	fileOffStart := indexSize + 4

	curOff := uint32(fileOffStart)
	curIndexOff := int64(4)
	for _, ent := range fileList {
		stm, err2 := ioutil.ReadFile(path.Join(dirname, ent.name))
		if err2 != nil {
			fmt.Printf("Can't open %s, err:%v", ent.name, err2)
			return false
		}
		newstm := lzssPack(stm)

		fs.Seek(curIndexOff, os.SEEK_SET)
		nameLen := []byte{uint8(len(ent.nameEncoded))}
		fs.Write(nameLen)
		fs.Write(ent.nameEncoded)
		serEnt := fileEntrySerialized{uint32(len(newstm)), uint32(ent.size), curOff}
		binary.Write(fs, binary.BigEndian, &serEnt)
		curIndexOff += int64(len(ent.nameEncoded) + 1 + 12)

		fs.Seek(int64(curOff), os.SEEK_SET)
		fs.Write(newstm)
		curOff += uint32(len(newstm))
	}

	return true
}

func parseCp(s string) int {
	switch s {
	case "932":
		return codec.C932
	case "936":
		return codec.C936
	default:
		panic("error code page")
	}
}
func usage() {
	fmt.Printf("Usage: %s <e|p> <arcName> <dirName> {nameEncoding}\n", os.Args[0])

}
func main() {
	if len(os.Args) <= 4 {
		usage()
		return
	}
	if os.Args[1] == "p" {
		if len(os.Args) != 5 {
			usage()
			return
		}
		cp := parseCp(os.Args[4])
		packArc(os.Args[2], os.Args[3], cp)
	} else {
		fmt.Println("not support extract")
	}
}
