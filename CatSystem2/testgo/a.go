package main

import (
	"log"
	"math/bits"
	"math/rand"

	"github.com/regomne/bstream"
)

func unpackVal(in byte) (out byte) {
	out = in >> 1
	if (in & 1) != 0 {
		out ^= 0xff
	}
	return
}
func packVal(in byte) (out byte) {
	if in&0x80 == 0 {
		return in << 1
	}
	return (in << 1) ^ 0xff
}

func testPack() {
	for i := 0; i < 256; i++ {
		a := unpackVal(byte(i))
		b := packVal(a)
		if byte(i) != b {
			log.Printf("pack error: %d %d %d\n", i, a, b)
			return
		}
	}
	log.Println("pack pass")
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

func unQueryTable(v1, v2, v3, v4 byte) uint32 {
	table := getFilterTable()
	val := table[0][v1]
	val |= table[1][v2]
	val |= table[2][v3]
	val |= table[3][v4]
	return val
}

func queryTable(val uint32) (v1, v2, v3, v4 byte) {
	v4 = byte((val & 3) | (val>>6)&0xc | (val>>12)&0x30 | (val>>18)&0xc0)
	v3 = byte(((val >> 2) & 3) | (val>>8)&0xc | (val>>14)&0x30 | (val>>20)&0xc0)
	v2 = byte(((val >> 4) & 3) | (val>>10)&0xc | (val>>16)&0x30 | (val>>22)&0xc0)
	v1 = byte(((val >> 6) & 3) | (val>>12)&0xc | (val>>18)&0x30 | (val>>24)&0xc0)
	return
}

func testQueryTable() {
	for i := 0; i < 100; i++ {
		n := rand.Uint32()
		v1, v2, v3, v4 := queryTable(n)
		n2 := unQueryTable(v1, v2, v3, v4)
		if n != n2 {
			log.Printf("query error: ori: %d query: %d %d %d %d ret: %d\n",
				n, v1, v2, v3, v4, n2)
			return
		}
	}
	log.Println("query pass")
}
func readBitLen(stm *bstream.BStream) uint32 {
	digits := 0
	val := uint32(0)
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

func testBitLen() {
	s := bstream.NewBStreamWriter(100)
	vals := make([]uint32, 100)
	for i := 0; i < 100; i++ {
		v := rand.Uint32()
		if v == 0 {
			continue
		}
		vals[i] = v
		writeBitLen(s, v)
	}

	r := bstream.NewBStreamReader(s.Bytes())
	for i := 0; i < 100; i++ {
		v := vals[i]
		if v == 0 {
			continue
		}
		v2 := readBitLen(r)
		if v != v2 {
			log.Printf("bitLen error:%d %d\n", v, v2)
			return
		}
	}
	log.Println("bitLen pass")
}

func main() {
	testPack()
	testQueryTable()
	testBitLen()
}
