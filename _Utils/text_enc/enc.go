package main

import "fmt"

import "log"
import "io/ioutil"
import "crypto/rand"
import "os"

const KeyLen = 4096

func encTxt(key []uint8, fname string) {
	bf, err := ioutil.ReadFile(fname)
	if err != nil {
		log.Fatalf("error: %v", err)
	}
	for i := 0; i < len(bf); i++ {
		bf[i] ^= key[i&0x3ff]
	}
	fp, err1 := os.Create(fname)
	if err1 != nil {
		log.Fatalf("error: %v", err1)
	}
	fp.Write(bf)
	fp.Truncate(int64(len(bf)))
	fp.Close()
}

const EncKeyTag = "8ugjiyhz"

func xorKey(key []uint8) {
	for i := 0; i < len(key); i++ {
		key[i] ^= EncKeyTag[i&7]
	}
}

func genKey(keyfile string) (key []uint8) {
	key = make([]uint8, KeyLen)
	rand.Read(key)

	var newKey = []uint8(key)
	xorKey(newKey)

	fs, err := os.Create(keyfile)
	if err != nil {
		log.Fatalf("error: %v", err)
	}
	fs.Write(newKey)
	fs.Close()
	return
}

func main() {
	if len(os.Args) < 3 {
		fmt.Printf("usage1: %s key <keyfile>\n", os.Args[0])
		fmt.Printf("usage2: %s enc <keyfile> <txtfile>\n", os.Args[0])
		return
	}

	if os.Args[1] == "key" {
		genKey(os.Args[2])
		return
	} else if os.Args[1] == "enc" {
		key, err := ioutil.ReadFile(os.Args[2])
		if err != nil {
			log.Fatalf("Can't open key file. %v", err)
		}
		xorKey(key)
		encTxt(key, os.Args[3])
	}
}
