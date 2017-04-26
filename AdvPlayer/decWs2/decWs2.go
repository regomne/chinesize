package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

const ws2Path = `f:\chinesize\AdvPlayer\decWs2\Rio2\`
const ws2PathN = `f:\chinesize\AdvPlayer\decWs2\Rio2Dec\`

func decWs2(stm []byte) {
	for i := 0; i < len(stm); i++ {
		stm[i] = (stm[i] << 6) | (stm[i] >> 2)
	}
}

func main() {
	if len(os.Args) != 3 {
		fmt.Printf("usage: %s <ori_path> <dec_path>\n", os.Args[0])
		return
	}
	os.MkdirAll(os.Args[2], os.ModeDir)
	filepath.Walk(os.Args[1], func(path string, info os.FileInfo, err error) error {
		if info.IsDir() {
			return nil
		}
		bf1, err := ioutil.ReadFile(path)
		if err != nil {
			log.Printf("read file %s error:%v\n", path, err)
			return err
		}
		decWs2(bf1)
		fname2 := filepath.Join(os.Args[2], filepath.Base(path))
		err = ioutil.WriteFile(fname2, bf1, os.ModePerm)
		if err != nil {
			log.Printf("write file %s error:%v\n", path, err)
			return err
		}
		return nil
	})
}
