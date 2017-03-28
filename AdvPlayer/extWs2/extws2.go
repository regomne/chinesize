package main

import (
	"fmt"
	"strconv"
	"strings"

	"github.com/regomne/eutil/pehelper"
)

func parsePattern(pattern string) (isIgnore []bool, patt []byte) {
	bf := strings.Split(pattern, " ")
	isIgnore = make([]bool, len(bf))
	patt = make([]byte, len(bf))
	for idx, by := range bf {
		if by == "??" {
			isIgnore[idx] = true
		} else {
			b, err := strconv.ParseUint(by, 16, 8)
			if err != nil {
				panic(err)
			}
			patt[idx] = byte(b)
		}
	}
	return
}

func findBytes(buf []byte, pattern string) (uint32, error) {
	isIgnore, patt := parsePattern(pattern)
	for i := 0; i < len(buf)-len(patt); i++ {
		match := true
		for j := 0; j < len(patt); j++ {
			if isIgnore[j] {
				continue
			}
			if patt[j] != buf[i+j] {
				match = false
				break
			}
		}
		if match {
			return uint32(i), nil
		}
	}
	return 0, fmt.Errorf("can't find pattern")
}

func readInstInfo(image []byte, startOff uint32) {

}

func main() {
	image, _, err := peHelper.LoadPEImage(`d:\galgame\想いを捧げる乙女のメロディー\AdvHD2.exe`)
	if err != nil {
		fmt.Println(err)
		return
	}
	var off uint32
	off, err = findBytes(image, "8b 2c 85 ?? ?? ?? ?? 85 ed 0f 84 ?? ?? ?? ?? 38 5d 00")
	if err != nil {
		fmt.Println(err)
		return
	}

}
