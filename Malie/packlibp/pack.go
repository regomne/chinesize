package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"path"
)

type Entry struct {
	FileName    [20]byte
	Flags       uint32
	OffsetIndex uint32
	Length      uint32
}

type LibpIndex struct {
	index    []Entry
	rootPath string
}

func (libp *LibpIndex) packDir(path1 string) {
	eles, err := ioutil.ReadDir(path1)
	if err != nil {
		log.Fatal(err)
	}

	dirs := []string{}
	files := []string{}
	for _, ele := range eles {
		if ele.IsDir() {
			dirs = append(dirs, ele.Name())
		} else {
			files = append(files, ele.Name())
		}
	}

	var ent Entry
	copy(ent.FileName[:], []byte(path.Base(path1)))
	ent.Flags = 0
	ent.OffsetIndex = 0
	ent.Length = uint32(len(dirs) + len(files))

}

func main() {
	fmt.Printf("over\n")
}
