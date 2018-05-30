package main

type YbnHeader struct {
	Magic        [4]byte
	Version      uint32
	InstCnt      uint32
	CodeSize     uint32
	ArgSize      uint32
	ResourceSize uint32
	OffSize      uint32
	Resv         uint32
}

type YInst struct {
	Op     uint8
	ArgCnt uint8
	Unk    uint16
}

type YArg struct {
	Value     uint16
	Type      uint16
	ResSize   uint32
	ResOffset uint32
}

type YResInfo struct {
	Type uint8
	Len  uint16
}
