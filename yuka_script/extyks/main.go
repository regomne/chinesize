package main

type YksHeader struct {
	Magic      [8]byte
	HeaderSize uint32
	Unk1       uint32
	PrgOffset  uint32
	PrgCnt     uint32
	CmdOffset  uint32
	CmdCnt     uint32
	ResOffset  uint32
	ResCnt     uint32
	Unk2       uint32
	Unk3       uint32
}

const (
	STR    = 1
	DWORD  = 2
	FLAG   = 3
	VALUE  = 4
	ZERO   = 5
	MINUS1 = 6
)

func main() {

}
