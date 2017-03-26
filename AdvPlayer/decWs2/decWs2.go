package main

import (
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

const ws2Path = `f:\chinesize\AdvPlayer\extArc\Rio\`
const ws2PathN = `f:\chinesize\AdvPlayer\extArc\RioDec\`

func decWs2(stm []byte) {
	for i := 0; i < len(stm); i++ {
		stm[i] = (stm[i] << 6) | (stm[i] >> 2)
	}
}

func main() {
	filepath.Walk(ws2Path, func(path string, info os.FileInfo, err error) error {
		if info.IsDir() {
			return nil
		}
		bf1, err := ioutil.ReadFile(path)
		if err != nil {
			log.Printf("read file %s error:%v\n", path, err)
			return err
		}
		decWs2(bf1)
		fname2 := filepath.Join(ws2PathN, filepath.Base(path))
		err = ioutil.WriteFile(fname2, bf1, os.ModePerm)
		if err != nil {
			log.Printf("write file %s error:%v\n", path, err)
			return err
		}
		return nil
	})
}
