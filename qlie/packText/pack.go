package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"

	"github.com/regomne/eutil/codec"

	"github.com/regomne/eutil/textFile"
)

func lineHasFullAngle(l string) bool {
	for _, c := range l {
		if int(c) > 0x80 {
			return true
		}
	}
	return false
}

func combineText(ori, tran []string) (ns []string) {
	ns = make([]string, 0, len(ori))
	curNewIdx := 0
	for _, l := range ori {
		l2 := strings.Trim(l, " 　\t")
		if len(l2) == 0 {
			ns = append(ns, l)
			continue
		}
		runes2 := []rune(l2)
		if l2[0] == '^' {
			if strings.Index(l2, "^savescene,") == 0 ||
				strings.Index(l2, "^select,") == 0 {
				ns = append(ns, tran[curNewIdx])
				curNewIdx++
			} else {
				ns = append(ns, l)
			}
		} else if runes2[0] == '％' || l2[0] == '\\' {
			ns = append(ns, l)
		} else if lineHasFullAngle(l2) {
			runes := []rune(l)
			if runes[0] != '　' {
				ns = append(ns, tran[curNewIdx])
			} else {
				ns = append(ns, string('　')+tran[curNewIdx])
			}
			curNewIdx++
		} else {
			ns = append(ns, l)
		}
	}
	return ns
}

func main() {
	if len(os.Args) < 4 {
		fmt.Printf("usage: %v <origin_dir> <txt_dir> <output_dir>\n", os.Args[0])
		return
	}
	oriDir, txtDir, outputDir := os.Args[1], os.Args[2], os.Args[3]
	filepath.Walk(oriDir, func(fname string, info os.FileInfo, err error) error {
		if err != nil {
			fmt.Println("error", err)
		}
		relpath, _ := filepath.Rel(oriDir, fname)
		if strings.Index(relpath, ".s") != -1 {
			relTxtName := relpath[:len(relpath)-2] + ".txt"
			txtName := filepath.Join(txtDir, relTxtName)
			if _, err := os.Stat(txtName); os.IsNotExist(err) {
				return nil
			}
			ls1, err := textFile.ReadWin32TxtToLines(fname)
			if err != nil {
				return err
			}
			ls2, err := textFile.ReadWin32TxtToLines(txtName)
			if err != nil {
				return err
			}

			nls := combineText(ls1, ls2)

			outName := filepath.Join(outputDir, relpath)
			outDirName := filepath.Dir(outName)
			if _, err := os.Stat(outDirName); os.IsNotExist(err) {
				os.MkdirAll(outDirName, os.ModeDir)
			}
			stm := codec.Encode(strings.Join(nls, "\r\n"), codec.UTF16LE, codec.Replace)
			err = ioutil.WriteFile(outName, stm, 0644)
			if err != nil {
				return err
			}
		}
		return nil
	})
}
