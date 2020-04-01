package main

import (
	"bytes"
	"compress/zlib"
	"encoding/binary"
	"encoding/json"
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"

	"github.com/regomne/bstream"
)

// HG3Header hg3 file header
type HG3Header struct {
	Magic      [4]byte
	HeaderSize uint32
	Info       uint32
}

// ImageBlockInfo hg3 consists of some blocks
type ImageBlockInfo struct {
	NextOffset uint32
	ImageID    uint32
}

// SectionInfo block consists of some sections
type SectionInfo struct {
	Tag        [8]byte
	NextOffset uint32
	SecLength  uint32
}

// SecStdinfo stdinfo section
type SecStdinfo struct {
	Width        uint32
	Height       uint32
	BitDepth     uint32
	OffsetX      int32
	OffsetY      int32
	CanvasWidth  uint32
	CanvasHeight uint32
	Unk1         uint32
	Unk2         uint32
	Unk3         uint32
}

// SecImg img000XX section
type SecImg struct {
	Unk1        uint32
	Height      uint32
	DataCompLen uint32
	DataUncLen  uint32
	CtrlCompLen uint32
	CtrlUncLen  uint32
}

type secRecord struct {
	Tag  string
	Info interface{}
	Data interface{}
}

type blockRecord struct {
	ImageID  uint32
	Sections []secRecord
}

type hg3Record struct {
	ImageBaseName string
	Records       []blockRecord
}

func unpackVal(in byte) (out byte) {
	out = in >> 1
	if (in & 1) != 0 {
		out ^= 0xff
	}
	return
}

var gTbl [4][256]uint32
var gTblInited = false

func getFilterTable() (tbl [][256]uint32) {
	if !gTblInited {
		for i := uint32(0); i < 256; i++ {
			val := i & 0xc0
			val <<= 6
			val |= i & 0x30
			val <<= 6
			val |= i & 0xc
			val <<= 6
			val |= i & 3
			gTbl[3][i] = val
			gTbl[2][i] = val << 2
			gTbl[1][i] = val << 4
			gTbl[0][i] = val << 6
		}
		gTblInited = true
	}
	return gTbl[:]
}

func unDeltaFilter(buff []byte, width, height, depthBytes uint32) (out []byte) {
	if depthBytes != 4 && depthBytes != 3 {
		log.Fatalf("depth bytes error:%d", depthBytes)
	}
	table := getFilterTable()
	secLen := len(buff) / 4
	out = make([]byte, len(buff))
	for i := 0; i < secLen; i++ {
		val := table[0][buff[i]]
		val |= table[1][buff[i+secLen]]
		val |= table[2][buff[i+secLen*2]]
		val |= table[3][buff[i+secLen*3]]

		out[i*4] = unpackVal(byte(val))
		out[i*4+1] = unpackVal(byte(val >> 8))
		out[i*4+2] = unpackVal(byte(val >> 16))
		out[i*4+3] = unpackVal(byte(val >> 24))
	}

	stride := width * depthBytes

	for x := uint32(depthBytes); x < stride; x++ {
		out[x] += out[x-depthBytes]
	}

	for y := uint32(1); y < height; y++ {
		line := out[y*stride:]
		prev := out[(y-1)*stride:]
		for x := uint32(0); x < stride; x++ {
			line[x] += prev[x]
		}
	}
	return
}

func unRLE(buff []byte, ctrl []byte) (out []byte) {
	stm := bstream.NewBStreamReader(ctrl)
	getLen := func() int {
		digits := 0
		val := 0
		for {
			b, err := stm.ReadBit()
			if b {
				break
			}
			if err != nil {
				panic("error")
			}
			digits++
		}
		val = 1 << uint32(digits)
		for digits--; digits >= 0; digits-- {
			b, _ := stm.ReadBit()
			if b {
				val |= (1 << uint32(digits))
			}
		}
		return val
	}

	copyFlag, _ := stm.ReadBit()
	outLen := getLen()
	out = make([]byte, outLen)
	n := 0
	bufPos := 0
	for i := 0; i < outLen; i += n {
		n = getLen()
		if copyFlag {
			copy(out[i:i+n], buff[bufPos:bufPos+n])
			bufPos += n
		}
		copyFlag = !copyFlag
	}
	return
}

func unzlib(reader io.Reader, comprLen, uncomprLen uint32) (out []byte, err error) {
	compData := make([]byte, comprLen)
	_, err = reader.Read(compData)
	if err != nil {
		return
	}
	byReader := bytes.NewReader(compData)
	zReader, _ := zlib.NewReader(byReader)
	out, err = ioutil.ReadAll(zReader)
	if err != nil {
		return
	}
	zReader.Close()
	if int(uncomprLen) != len(out) {
		err = fmt.Errorf("uncompress len error")
	}
	return
}

func dumpObj(name string, buff []byte) {
	fs, err := os.Create(name)
	if err == nil {
		fs.Write(buff)
		fs.Close()
	}
}

func write32DibToPng(dib []byte, width, height int, out io.Writer) (err error) {
	if len(dib) < int(width*height*4) {
		err = fmt.Errorf("dib size error")
		return
	}
	canvas := image.NewNRGBA(image.Rect(0, 0, width, height))
	stride := width * 4
	for y := 0; y < height; y++ {
		line := dib[y*stride:]
		for x := 0; x < width; x++ {
			canvas.Set(x, height-y-1, color.NRGBA{
				B: line[x*4],
				G: line[x*4+1],
				R: line[x*4+2],
				A: line[x*4+3],
			})
		}
	}
	err = png.Encode(out, canvas)
	return
}

func write24DibToPng(dib []byte, width, height int, out io.Writer) (err error) {
	if len(dib) < int(width*height*3) {
		err = fmt.Errorf("dib size error")
		return
	}
	canvas := image.NewNRGBA(image.Rect(0, 0, width, height))
	stride := width * 3
	for y := 0; y < height; y++ {
		line := dib[y*stride:]
		for x := 0; x < width; x++ {
			canvas.Set(x, height-y-1, color.NRGBA{
				B: line[x*3],
				G: line[x*3+1],
				R: line[x*3+2],
				A: 255,
			})
		}
	}
	err = png.Encode(out, canvas)
	return
}

func extractAnImage(reader io.Reader, stdInfo *SecStdinfo, imgInfo *SecImg,
	out io.Writer) (err error) {
	if stdInfo.BitDepth != 32 && stdInfo.BitDepth != 24 {
		err = fmt.Errorf("only support 32/24 bit image, this is %d", stdInfo.BitDepth)
		return
	}
	if stdInfo.Height != imgInfo.Height {
		err = fmt.Errorf("height not fit")
		return
	}
	uncData, err := unzlib(reader, imgInfo.DataCompLen, imgInfo.DataUncLen)
	if err != nil {
		return
	}
	uncCtrl, err := unzlib(reader, imgInfo.CtrlCompLen, imgInfo.CtrlUncLen)
	if err != nil {
		return
	}
	dib1 := unRLE(uncData, uncCtrl)
	dib2 := unDeltaFilter(dib1, stdInfo.Width, stdInfo.Height, stdInfo.BitDepth/8)
	if stdInfo.BitDepth == 32 {
		err = write32DibToPng(dib2, int(stdInfo.Width), int(stdInfo.Height), out)
	} else {
		err = write24DibToPng(dib2, int(stdInfo.Width), int(stdInfo.Height), out)
	}
	return
}

func findStr(bf []byte) string {
	idx := bytes.IndexByte(bf, 0)
	if idx == -1 {
		return string(bf)
	}
	return string(bf[0:idx])
}

func extractABlock(reader io.Reader, blockID uint32, namePrefix string) (
	blockInfo []secRecord, err error) {
	var secInfo SectionInfo
	binary.Read(reader, binary.LittleEndian, &secInfo)
	if !bytes.Equal(secInfo.Tag[:], []byte("stdinfo\x00")) {
		err = fmt.Errorf("first section must be stdinfo")
		return
	}
	blockInfo = make([]secRecord, 0, 3)

	var stdInfo SecStdinfo
	err = binary.Read(reader, binary.LittleEndian, &stdInfo)
	if err != nil {
		return
	}
	blockInfo = append(blockInfo, secRecord{"stdinfo", stdInfo, nil})

	secImgIdx := 0
	for secInfo.NextOffset != 0 {
		err = binary.Read(reader, binary.LittleEndian, &secInfo)
		if err != nil {
			return
		}

		outName := fmt.Sprintf("%s_%d_%03d", namePrefix, blockID, secImgIdx)
		secImgIdx++
		if bytes.Equal(secInfo.Tag[0:4], []byte("img0")) &&
			!bytes.Equal(secInfo.Tag[0:7], []byte("img_jpg")) {
			var imgInfo SecImg
			err = binary.Read(reader, binary.LittleEndian, &imgInfo)
			if err != nil {
				return
			}
			var fs *os.File
			fs, err = os.Create(outName + ".png")
			if err != nil {
				return
			}
			defer fs.Close()
			err = extractAnImage(reader, &stdInfo, &imgInfo, fs)
			if err != nil {
				return
			}
			blockInfo = append(blockInfo, secRecord{findStr(secInfo.Tag[:]), imgInfo, nil})
		} else {
			data := make([]byte, secInfo.SecLength)
			_, err = reader.Read(data)
			if err != nil {
				return
			}
			blockInfo = append(blockInfo, secRecord{findStr(secInfo.Tag[:]), nil, data})
		}
	}
	return
}

func extractHG3(fname, destDir string) bool {
	fs, err := os.Open(fname)
	if err != nil {
		fmt.Printf("open hg3 file error:%v\n", err)
		return false
	}
	defer fs.Close()
	if _, err = os.Stat(destDir); os.IsNotExist(err) {
		os.MkdirAll(destDir, os.ModePerm)
	}
	basename := filepath.Base(fname)
	idx := strings.Index(basename, ".")
	prefix := destDir
	if idx != -1 {
		prefix = filepath.Join(prefix, basename[0:idx])
	} else {
		prefix = filepath.Join(prefix, basename)
	}

	var fileHeader HG3Header
	binary.Read(fs, binary.LittleEndian, &fileHeader)
	if !bytes.Equal(fileHeader.Magic[:], []byte("HG-3")) ||
		fileHeader.HeaderSize != 12 {
		fmt.Println("not a hg3 file")
		return false
	}
	blocks := make([]blockRecord, 0)

	for {
		curOffset, _ := fs.Seek(0, 1)
		var blockInfo ImageBlockInfo
		err = binary.Read(fs, binary.LittleEndian, &blockInfo)
		if err != nil {
			fmt.Printf("read file error:%v\n", err)
			return false
		}
		var block []secRecord
		block, err = extractABlock(fs, uint32(len(blocks)), prefix)
		if err != nil {
			fmt.Printf("extract block id:%d error:%v\n", len(blocks), err)
			return false
		}

		blocks = append(blocks, blockRecord{blockInfo.ImageID, block})

		if blockInfo.NextOffset == 0 {
			break
		}
		fs.Seek(curOffset+int64(blockInfo.NextOffset), 0)
	}

	fileRecord := hg3Record{basename, blocks}
	fileInfo, err := json.MarshalIndent(fileRecord, "", "  ")
	if err != nil {
		fmt.Printf("json error:%v\n", err)
		return false
	}
	fsJson, err := os.Create(filepath.Join(destDir, "info.json"))
	if err != nil {
		fmt.Printf("can't create json file:%v\n", err)
		return false
	}
	defer fsJson.Close()
	fsJson.Write(fileInfo)
	return true
}

func main() {
	exitCode := 0
	defer func() {
		os.Exit(exitCode)
	}()
	isExtract := flag.Bool("e", false, "extract hg3")
	isPack := flag.Bool("p", false, "pack hg3")
	inputName := flag.String("i", "", "hg3 file name(with -e) or png image folder(with -p)")
	outputName := flag.String("o", "", "output name")
	flag.Parse()
	if (*isExtract && *isPack) || (!*isExtract && !*isPack) ||
		*inputName == "" {
		flag.Usage()
		return
	}

	output := *outputName
	if output == "" {
		if *isExtract {
			idx := strings.LastIndex(*inputName, ".")
			if idx != -1 && strings.LastIndexAny(*inputName, "\\/") < idx {
				output = (*inputName)[:idx]
			} else {
				fmt.Printf("please specify the output folder!\n")
				return
			}
		} else {
			output = *inputName + ".hg3"
		}
	}

	hasError := false
	if *isExtract {
		hasError = extractHG3(*inputName, output)
		fmt.Printf("complete")
	} else {
		hasError = packImagesToHG3(*inputName, output)
		fmt.Printf("complete")
	}
	if hasError {
		exitCode = 1
	}
}
