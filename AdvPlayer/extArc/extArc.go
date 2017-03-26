package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path"

	"github.com/regomne/eutil/codec"
)

const arcName = `d:\galgame\想いを捧げる乙女のメロディー\Rio.arc`
const extPath = `Rio`

var le = binary.LittleEndian

func readUstr(fs io.Reader) string {
	s := bytes.NewBuffer([]byte{})
	var ch [2]byte
	for {
		_, err := fs.Read(ch[:])
		if err != nil {
			panic(err)
		}
		if ch[0] == 0 && ch[1] == 0 {
			return codec.Decode(s.Bytes(), codec.UTF16LE)
		}
		s.Write(ch[:])
	}
}

type entryS struct {
	Size uint32
	Off  uint32
	Name string
}
type hdrS struct {
	Cnt       uint32
	IndexSize uint32
}

func readHdr(fs io.Reader) (hdr hdrS) {
	binary.Read(fs, le, &hdr)
	return
}

func readIndex(fs io.Reader, hdr *hdrS) []entryS {
	entries := make([]entryS, hdr.Cnt)
	for i := uint32(0); i < hdr.Cnt; i++ {
		var ent entryS
		binary.Read(fs, le, &ent.Size)
		binary.Read(fs, le, &ent.Off)
		ent.Name = readUstr(fs)
		entries[i] = ent
	}
	return entries
}

func extFiles(path1 string, hdr *hdrS, entries []entryS, fs io.ReadSeeker) {
	offBase := hdr.IndexSize + uint32(binary.Size(*hdr))
	for i := 0; i < len(entries); i++ {
		entry := &entries[i]
		fname := path.Join(path1, entry.Name)
		fs.Seek(int64(offBase+entry.Off), 0)
		buff := make([]byte, entry.Size)
		_, err := fs.Read(buff)
		if err != nil {
			log.Printf("reading %s error: %v\n", fname, err)
			break
		}
		err = ioutil.WriteFile(fname, buff, os.ModePerm)
		if err != nil {
			log.Printf("writing file error:%v\n", err)
			break
		}
	}
}

func main() {
	fs, err := os.Open(arcName)
	if err != nil {
		panic(err)
	}
	hdr := readHdr(fs)
	entries := readIndex(fs, &hdr)
	curOff, _ := fs.Seek(0, 1)
	if int(curOff) != int(hdr.IndexSize)+binary.Size(hdr) {
		panic(fmt.Errorf("index size error"))
	}
	log.Printf("hdr:%v, entryCnt:%d\n", hdr, len(entries))
	extFiles(`Rio`, &hdr, entries, fs)
	log.Println("Complete.")
}
