package main

import (
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"
	"github.com/regomne/eutil/codec"
	"os"
	"strconv"
	"strings"
)

var gIsOutputOpcode bool
var gVerbose bool

func logf(fmts string, args ...interface{}) {
	if gVerbose {
		fmt.Printf(fmts, args...)
	}
}

func logln(args ...interface{}) {
	if gVerbose {
		fmt.Println(args...)
	}
}

func IndexOf(a []string, s string) int {
	for k, i := range a {
		if i == s {
			return k
		}
	}
	return 0
}

func includes(a []string, v string) bool {
	for _, i := range a {
		if i == v {
			return true
		}
	}
	return false
}

func decryptBlock(stm []byte, key []byte) {
	if len(key) != 4 {
		panic("key length error")
	}
	for i := 0; i < len(stm); i++ {
		stm[i] ^= key[i&3]
	}
	b := stm[0]
	stm[0] = b
}

func printUsage(exeName string) {
	fmt.Println("YBN extractor v3.0")
	fmt.Printf("Usage: %s -e -input <ybn> [-json <json>] [-txt <txt>] [options]\n", exeName)
	fmt.Printf("Usage: %s -p -input <ybn> -txt <txt> -new-ybn <new_ybn> [options]\n", exeName)
	flag.Usage()

	fmt.Printf(`
About the extraction to different formats:
  Repacking is only possible from a txt file generated from a ystXXXXX.ybn,
  ysc.ybn or yse.ybn file.
  Some ybn variants may only support specific file formats:
      YSCM:	json,	instruct,	txt
      YSER:	json,			txt
      YSLB:	json,	instruct,	txt
      YSTB:	json,	instruct,	txt,	decrypt
      YSTD:	json,	instruct
      YSTL:	json,	instruct
      YSVR:	json

  Different formats may contain different data depending on the variant.
  Which formats are supported is decided based on usefulness. In
  General one can say that (1) json files should contain as much as
  possible (if possible 1:1 binary reconstruction), (2) instruct files
  should aim to imitate source code files, (3) txt files should be used for
  translation purposes and therefore only contain strings and (4) decrypt
  files should be exactly only the original files without encryption.
  

About the key:
  Since the used XOR cipher is easy to break once you have some content like a
  string. Most games use the default key, but if not, you may try:
  0x6cfddadb or 0x30731B78. The Program may CRASH or PANIC if not given the
  correct key. If nothing works, try https://wiremask.eu/tools/xor-cracker/

About the opcode:
  This program can guess the opcode-msg and opcode-call which is needed, but
  you need to give it a .ybn which has some msg texts to do it. Generally, you
  can give it yst0XXXX.ybn where the XXXX is the maximum number among all the
  ybn file names.
  You can use:

  %s -e -input yst0XXXX.ybn -output-opcode

  to output the opcodes, and then use them in all the .ybn files of this game.
  You can also use the -op-other to use other ops to extract or pack text.
`, exeName)
}

func parseCmdOps(cds string) (err error, ops [256]string) {
	cdes := strings.Split(cds, ",")
	for i := 0; i < len(cdes); i++ {
		spl := strings.Split(cdes[i], ":")
		if len(spl) != 2 {
			err = fmt.Errorf("malformatted Op-Codes")
			return
		}
		opcode, e := strconv.Atoi(spl[0])
		if e != nil {
			return e, ops
		}
		if opcode > 255 {
			err = fmt.Errorf("malformatted Op-Codes")
			return
		}
		ops[uint8(opcode)] = spl[1]
	}
	return
}

func parseCp(s string) int {
	switch s {
	case "936":
		return codec.C936
	case "932":
		return codec.C932
	default:
		return codec.Unknown
	}
}

func extractYbnFile(ybnName, outJsonName, outTxtName, outInstructName, outDecryptName string, key []byte, ops *[256]string, codePage int) bool {
	logln("reading file:", ybnName)
	oriStm, err := os.ReadFile(ybnName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	stm := bytes.NewReader(oriStm)
	magic := make([]byte, 4)
	binary.Read(stm, binary.LittleEndian, &magic)
	switch strings.ToUpper(string(magic[:])) {
	case "YSTB":
		return parseYstbFile(oriStm, outJsonName, outTxtName, outDecryptName, outInstructName, key, ops, codePage)
	case "YSLB":
		return parseYslbFile(oriStm, outJsonName, outInstructName, codePage)
	case "YSCM":
		return parseYscmFile(oriStm, outJsonName, outTxtName, outInstructName, codePage)
	case "YSER":
		return parseYserFile(oriStm, outJsonName, outTxtName, codePage)
	case "YSTD":
		return parseYstdFile(oriStm, outJsonName, outInstructName, codePage)
	case "YSTL":
		return parseYstlFile(oriStm, outJsonName, outInstructName, codePage)
	case "YSVR":
		return parseYsvrFile(oriStm, outJsonName, codePage)
	default:
		fmt.Println("Unknown MAGIC-bytes")
		return false
	}
}

func packYbnFile(ybnName, outTxtName, outYbnName string, key []byte, ops *[256]string, codePage int) bool {
	logln("reading file:", ybnName)
	oriStm, err := os.ReadFile(ybnName)
	if err != nil {
		fmt.Println(err)
		return false
	}
	stm := bytes.NewReader(oriStm)
	magic := make([]byte, 4)
	binary.Read(stm, binary.LittleEndian, &magic)
	switch strings.ToUpper(string(magic[:])) {
	case "YSTB":
		return packYstbFile(oriStm, outTxtName, outYbnName, key, ops, codePage)
	case "YSCM":
		return packYscmFile(oriStm, outTxtName, outYbnName, codePage)
	case "YSER":
		return packYserFile(oriStm, outTxtName, outYbnName, codePage)
	default:
		fmt.Println("Unknown MAGIC-bytes or packing not supported")
		return false
	}
}

func main() {
	retCode := 0
	defer os.Exit(retCode)
	isExtract := flag.Bool("e", false, "extract a file")
	isPack := flag.Bool("p", false, "pack a ybn")
	inInputBinaryName := flag.String("input", "", "input ybn file name")
	outJsonName := flag.String("json", "", "output json file name")
	outInstructName := flag.String("instruct", "", "output instruct file name")
	outTxtName := flag.String("txt", "", "output txt file name")
	outDecryptName := flag.String("decrypt", "", "output decrypted file name")
	outYbnName := flag.String("new-ybn", "", "output ybn file name")
	keyInt := flag.Int64("key", 0x96ac6fd3, "decode key")
	codePage := flag.String("cp", "932", "specify code page")
	outputOpCode := flag.Bool("output-opcode", false, "output the opcode guessed")
	inOpCodes := flag.String("ops", "", "specify op-code names like 90:msg,29:call")
	verbose := flag.Bool("v", false, "verbose output")
	flag.Parse()
	key := [4]byte{}
	key[0] = byte(*keyInt & 0xff)
	key[1] = byte((*keyInt >> 8) & 0xff)
	key[2] = byte((*keyInt >> 16) & 0xff)
	key[3] = byte((*keyInt >> 24) & 0xff)
	var opCodes [256]string
	if *inOpCodes != "" {
		var err error
		err, opCodes = parseCmdOps(*inOpCodes)
		if err != nil {
			printUsage(os.Args[0])
			return
		}
	}
	gIsOutputOpcode = *outputOpCode
	gVerbose = *verbose
	if (*isExtract && *isPack) || (!*isExtract && !*isPack) ||
		((*isPack || *isExtract) && *inInputBinaryName == "") ||
		(*isPack && (*outYbnName == "" || *outTxtName == "")) {
		printUsage(os.Args[0])
		return
	}
	if *isExtract {
		extractYbnFile(*inInputBinaryName, *outJsonName, *outTxtName, *outInstructName, *outDecryptName, key[:], &opCodes, parseCp(*codePage))
	} else if *isPack {
		packYbnFile(*inInputBinaryName, *outTxtName, *outYbnName, key[:], &opCodes, parseCp(*codePage))
	} else {
		printUsage(os.Args[0])
		return
	}
}
