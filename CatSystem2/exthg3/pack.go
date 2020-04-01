package main

import (
	"bytes"
	"compress/zlib"
	"encoding/base64"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"image"
	"image/color"
	_ "image/png"
	"io"
	"io/ioutil"
	"math/bits"
	"os"
	"path/filepath"
	"strings"

	"github.com/regomne/bstream"
)

func getDibFromImage(img image.Image, depthBytes int) (dib []byte, err error) {
	bounds := img.Bounds()
	width, height := bounds.Max.X, bounds.Max.Y
	dibSize := width * depthBytes * height
	if dibSize < 1024 {
		dibSize = 1024
	}
	dib = make([]byte, dibSize)
	stride := width * depthBytes

	if depthBytes == 4 {
		for y := 0; y < height; y++ {
			line := dib[y*stride:]
			for x := 0; x < width; x++ {
				c := img.At(x, height-y-1)
				nc := color.NRGBAModel.Convert(c)
				r, g, b, a := nc.RGBA()
				line[x*4] = byte(b >> 8)
				line[x*4+1] = byte(g >> 8)
				line[x*4+2] = byte(r >> 8)
				line[x*4+3] = byte(a >> 8)
			}
		}
	} else if depthBytes == 3 {
		for y := 0; y < height; y++ {
			line := dib[y*stride:]
			for x := 0; x < width; x++ {
				c := img.At(x, height-y-1)
				nc := color.NRGBAModel.Convert(c)
				r, g, b, _ := nc.RGBA()
				line[x*3] = byte(b >> 8)
				line[x*3+1] = byte(g >> 8)
				line[x*3+2] = byte(r >> 8)
			}
		}
	} else {
		err = fmt.Errorf("only support depth bytes 4 or 3, given %d", depthBytes)
	}
	return
}

func diffImage(dib []byte, width, height, depthBytes uint32) []byte {
	stride := width * depthBytes
	for y := height - 1; y >= 1; y-- {
		line := dib[y*stride:]
		prev := dib[(y-1)*stride:]
		for x := 0; x < int(stride); x++ {
			line[x] -= prev[x]
		}
	}

	for x := stride; x >= depthBytes; x-- {
		dib[x] -= dib[x-depthBytes]
	}
	return dib
}

func packVal(in byte) (out uint32) {
	if in&0x80 == 0 {
		return uint32(in << 1)
	}
	return uint32((in << 1) ^ 0xff)
}

func queryTable(val uint32) (v1, v2, v3, v4 byte) {
	v4 = byte((val & 3) | (val>>6)&0xc | (val>>12)&0x30 | (val>>18)&0xc0)
	v3 = byte(((val >> 2) & 3) | (val>>8)&0xc | (val>>14)&0x30 | (val>>20)&0xc0)
	v2 = byte(((val >> 4) & 3) | (val>>10)&0xc | (val>>16)&0x30 | (val>>22)&0xc0)
	v1 = byte(((val >> 6) & 3) | (val>>12)&0xc | (val>>18)&0x30 | (val>>24)&0xc0)
	return
}

func rgbaCvt(dib []byte, width, height, depthBytes uint32) (out []byte) {
	out = make([]byte, len(dib))
	secLen := width * height * depthBytes / 4
	sec1 := out
	sec2 := out[secLen:]
	sec3 := out[secLen*2:]
	sec4 := out[secLen*3:]
	p := dib
	for i := 0; i < int(secLen); i++ {
		val := packVal(p[0]) | (packVal(p[1]) << 8) | (packVal(p[2]) << 16) | (packVal(p[3]) << 24)
		v1, v2, v3, v4 := queryTable(val)
		sec1[i] = v1
		sec2[i] = v2
		sec3[i] = v3
		sec4[i] = v4
		p = p[4:]
	}
	return
}

func writeBitLen(stm *bstream.BStream, l uint32) {
	if l == 0 {
		panic("error bit len")
	}
	bitCnt := bits.Len32(l)
	for i := bitCnt - 1; i != 0; i-- {
		stm.WriteBit(false)
	}
	l <<= uint(32 - bitCnt)
	for ; bitCnt != 0; bitCnt-- {
		stm.WriteBit(l&0x80000000 != 0)
		l <<= 1
	}
}

func doRLE(dib []byte) (content []byte, ctrl []byte) {
	stm := bstream.NewBStreamWriter(200)
	stm.WriteBit(true)
	writeBitLen(stm, uint32(len(dib)))
	writeBitLen(stm, uint32(len(dib)))
	content = dib
	ctrl = stm.Bytes()
	return
}

func doZlib(in []byte) (out []byte, err error) {
	var bf bytes.Buffer
	w := zlib.NewWriter(&bf)
	_, err = w.Write(in)
	if err != nil {
		return
	}
	w.Close()
	out = bf.Bytes()
	return
}

func readImageToBuff(stdInfo *SecStdinfo, imgInfo *SecImg, fname string) (
	newImgInfo SecImg, out []byte, err error) {
	fs, err := os.Open(fname)
	if err != nil {
		return
	}
	img, _, err := image.Decode(fs)
	fs.Close()
	if err != nil {
		err = fmt.Errorf("can't decode image: %v", err)
		return
	}

	if int(stdInfo.Width) != img.Bounds().Max.X ||
		int(stdInfo.Height) != img.Bounds().Max.Y {
		err = fmt.Errorf("size error, should be: %d*%d", stdInfo.Width, stdInfo.Height)
		return
	}

	dib, err := getDibFromImage(img, int(stdInfo.BitDepth/8))
	if err != nil {
		return
	}

	dib = diffImage(dib, stdInfo.Width, stdInfo.Height, stdInfo.BitDepth/8)
	dib = rgbaCvt(dib, stdInfo.Width, stdInfo.Height, stdInfo.BitDepth/8)
	content, ctrl := doRLE(dib)
	newImgInfo.DataUncLen = uint32(len(content))
	newImgInfo.CtrlUncLen = uint32(len(ctrl))
	content, err = doZlib(content)
	if err != nil {
		err = fmt.Errorf("compress error: %v", err)
		return
	}
	ctrl, err = doZlib(ctrl)
	if err != nil {
		err = fmt.Errorf("compress error: %v", err)
		return
	}
	newImgInfo.DataCompLen = uint32(len(content))
	newImgInfo.CtrlCompLen = uint32(len(ctrl))
	newImgInfo.Height = imgInfo.Height
	newImgInfo.Unk1 = imgInfo.Unk1
	out = append(content, ctrl...)
	return
}

func genBlockWithImage(block *blockRecord, blockID int, pathPrefix string) (out []byte, err error) {
	if block.Sections[0].Tag != "stdinfo" {
		err = fmt.Errorf("section 0 must be stdinfo")
		return
	}
	if len(block.Sections) < 2 {
		err = fmt.Errorf("need at least 2 seciton")
		return
	}
	var stdInfo SecStdinfo
	temp, _ := json.Marshal(block.Sections[0].Info)
	err = json.Unmarshal(temp, &stdInfo)
	if err != nil {
		err = fmt.Errorf("section 0 format error: %v", err)
		return
	}
	var secInfo SectionInfo
	copy(secInfo.Tag[:], []byte("stdinfo\x00"))
	secInfo.SecLength = uint32(binary.Size(stdInfo))
	secInfo.NextOffset = secInfo.SecLength + uint32(binary.Size(secInfo))

	var writer bytes.Buffer
	binary.Write(&writer, binary.LittleEndian, &secInfo)
	binary.Write(&writer, binary.LittleEndian, &stdInfo)
	for idx, sec := range block.Sections[1:] {
		secInfo = SectionInfo{}
		if sec.Tag[0:4] == "img0" && sec.Tag[0:7] != "img_jpg" {
			copy(secInfo.Tag[:], []byte(sec.Tag))
			var imgInfo SecImg
			temp, _ := json.Marshal(sec.Info)
			err = json.Unmarshal(temp, &imgInfo)
			if err != nil {
				err = fmt.Errorf("section %d info error: %v", idx+1, err)
				return
			}
			imgName := fmt.Sprintf(`%s_%d_%03d.png`, pathPrefix, blockID, idx)
			newImgInfo, imgbf, err1 := readImageToBuff(&stdInfo, &imgInfo, imgName)
			if err1 != nil {
				err = fmt.Errorf("reading pic: %s error: %v", imgName, err1)
				return
			}
			secInfo.SecLength = uint32(binary.Size(newImgInfo) + len(imgbf))
			if idx+1 != len(block.Sections)-1 {
				secInfo.NextOffset = secInfo.SecLength + uint32(binary.Size(secInfo))
			}
			binary.Write(&writer, binary.LittleEndian, &secInfo)
			binary.Write(&writer, binary.LittleEndian, &newImgInfo)
			writer.Write(imgbf)
		} else {
			copy(secInfo.Tag[:], []byte(sec.Tag))
			dataString, _ := sec.Data.(string)
			data, err1 := base64.StdEncoding.DecodeString(dataString)
			if err1 != nil {
				err = fmt.Errorf("section %d data format error", idx)
				return
			}
			secInfo.SecLength = uint32(len(data))
			if idx+1 != len(block.Sections)-1 {
				secInfo.NextOffset = secInfo.SecLength + uint32(binary.Size(secInfo))
			}
			binary.Write(&writer, binary.LittleEndian, &secInfo)
			writer.Write(data)
		}
	}
	out = writer.Bytes()
	return
}

func writeBlocks(w io.Writer, blocks []blockRecord, pathPrefix string) (err error) {
	var padding [12]byte
	for idx, block := range blocks {
		var buff []byte
		buff, err = genBlockWithImage(&block, idx, pathPrefix)
		if err != nil {
			err = fmt.Errorf("generate block %d err: %v", idx, err)
			return
		}
		alignedLen := len(buff)
		if alignedLen%8 != 0 {
			alignedLen = alignedLen&(^7) + 8
		}
		if idx == 0 {
			alignedLen += 4
		}
		var blockInfo ImageBlockInfo
		blockInfo.ImageID = block.ImageID
		if idx+1 != len(blocks) {
			blockInfo.NextOffset = uint32(alignedLen) + 8
		}
		binary.Write(w, binary.LittleEndian, &blockInfo)
		w.Write(buff)
		if alignedLen != len(buff) {
			w.Write(padding[0 : alignedLen-len(buff)])
		}
	}
	return
}

func packImagesToHG3(imagePath string, destName string) bool {
	desc, err := ioutil.ReadFile(filepath.Join(imagePath, "info.json"))
	if err != nil {
		fmt.Println("reading json error:", err)
		return false
	}
	var fileRecord hg3Record
	err = json.Unmarshal(desc, &fileRecord)
	if err != nil {
		fmt.Println("json parse error:", err)
		return false
	}
	idx := strings.Index(fileRecord.ImageBaseName, ".")
	prefix := imagePath
	if idx != -1 {
		prefix = filepath.Join(prefix, fileRecord.ImageBaseName[0:idx])
	} else {
		prefix = filepath.Join(prefix, fileRecord.ImageBaseName)
	}

	fs, err := os.Create(destName)
	if err != nil {
		fmt.Println("create hg3 error:", err)
		return false
	}
	defer fs.Close()
	header := HG3Header{[4]byte{'H', 'G', '-', '3'}, 12, 0x300}
	binary.Write(fs, binary.LittleEndian, &header)
	err = writeBlocks(fs, fileRecord.Records, prefix)
	if err != nil {
		fmt.Println("writing block error:", err)
		return false
	}
	return true
}
