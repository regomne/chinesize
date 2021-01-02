package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"path"
)

type PnaHeader struct {
	Magic  uint32
	Unk1   uint32
	Width  uint32
	Height uint32
	ImgCnt uint32
}

type PnaLayerInfo struct {
	Unk1       uint32
	FrameIndex uint32
	OffsetX    int32
	OffsetY    int32
	Width      uint32
	Height     uint32
	Unk2       [3]uint32
	RawSize    uint32
}

const HdrFileName = "_hdr.bin"

func extPna(fname string, outFolder string) (retErr error) {
	fs, err := os.Open(fname)
	if err != nil {
		retErr = err
		return
	}

	defer fs.Close()
	var hdr PnaHeader
	binary.Read(fs, binary.LittleEndian, &hdr)
	if hdr.Magic != 0x50414e50 { //"PNAP"
		retErr = fmt.Errorf("magic not fit")
		return
	}
	entries := make([]PnaLayerInfo, hdr.ImgCnt)
	retErr = binary.Read(fs, binary.LittleEndian, &entries)
	if retErr != nil {
		return
	}
	var hdrFile *os.File
	hdrFile, retErr = os.Create(path.Join(outFolder, HdrFileName))
	if retErr != nil {
		return
	}
	binary.Write(hdrFile, binary.LittleEndian, &hdr)
	binary.Write(hdrFile, binary.LittleEndian, &entries)
	hdrFile.Close()

	for idx, entry := range entries {
		if entry.RawSize != 0 {
			bf := make([]byte, entry.RawSize)
			_, retErr = fs.Read(bf)
			if retErr != nil {
				return
			}
			retErr = ioutil.WriteFile(path.Join(outFolder, fmt.Sprintf("%03d.png", idx)),
				bf, os.ModePerm)
			if retErr != nil {
				return
			}
		}
	}
	return
}

func packPna(inFolder string, fname string) (retErr error) {
	var hdrFile *os.File
	hdrFile, retErr = os.Open(path.Join(inFolder, HdrFileName))
	if retErr != nil {
		return
	}
	defer hdrFile.Close()
	var hdr PnaHeader
	binary.Read(hdrFile, binary.LittleEndian, &hdr)
	if hdr.Magic != 0x50414e50 { //"PNAP"
		retErr = fmt.Errorf("magic not fit")
		return
	}
	entries := make([]PnaLayerInfo, hdr.ImgCnt)
	retErr = binary.Read(hdrFile, binary.LittleEndian, &entries)
	if retErr != nil {
		return
	}

	var fs *os.File
	fs, retErr = os.Create(fname)
	if retErr != nil {
		return
	}
	defer fs.Close()
	fs.Seek(int64(binary.Size(hdr)+binary.Size(PnaLayerInfo{})*int(hdr.ImgCnt)), 0)
	for idx, entry := range entries {
		if entry.RawSize != 0 {
			pngName := fmt.Sprintf("%03d.png", idx)
			png, err := ioutil.ReadFile(path.Join(inFolder, pngName))
			if err != nil {
				return fmt.Errorf("can't read %s: %v", pngName, err)
			}
			entries[idx].RawSize = uint32(len(png))
			_, err = fs.Write(png)
			if err != nil {
				return fmt.Errorf("writing error when %s: %v", pngName, err)
			}
		}
	}

	fs.Seek(0, 0)
	binary.Write(fs, binary.LittleEndian, &hdr)
	binary.Write(fs, binary.LittleEndian, &entries)
	return
}

func main() {

	isExtract := flag.Bool("e", false, "extract pna")
	isPack := flag.Bool("p", false, "pack pna")
	folderName := flag.String("folder", "", "pna folder name")
	pnaName := flag.String("pna", "", "pna file name")
	flag.Parse()
	if (!*isExtract && !*isPack) || (*isExtract && *isPack) ||
		*folderName == "" || *pnaName == "" {
		flag.Usage()
		return
	}

	var err error
	if *isExtract {
		fmt.Println("extracting...")
		os.MkdirAll(*folderName, os.ModePerm)
		err = extPna(*pnaName, *folderName)
	} else {
		fmt.Println("packing...")
		err = packPna(*folderName, *pnaName)
	}
	if err != nil {
		fmt.Println(err)
	} else {
		fmt.Println("complete.")
	}
}
