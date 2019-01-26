#encoding=utf-8
import hcbParser
import bytefile
import os

fnt={
    0x00000004:"クロ",
    0x000000DC:"ハル",
    0x000001B4:"千和",
    0x0000028C:"姫織",
    0x00000362:"ましろ",
    0x0000043A:"十夜",
    0x00000512:"あさひ",
    0x000005EA:"あず咲",
    0x000006C2:"ナナ",
    0x0000079A:"智仁",
    0x00000872:"ソル",
    0x000009C0:"ソル",
    0x000009D0:"ナハト",
    0x00000AB0:"女の子",
    0x00000B86:"ハル",
    0x00000C5E:"サブ１",
    0x00000D3A:"渡部",
    0x00000E10:"サブ３",
    0x00000F2B:"サブ４",
    0x00001046:"サブ５",
    0x00001184:"サブ６",
    0x00001287:"サブ７",
    0x0000139A:"母",
    0x00001413:"女の人",
    0x0000148C:"幼女",
    0x00001505:"母親",
    0x0000157E:"猫",
    0x000015F7:"蝶ネクタイをしたすまし顔の猫",
    0x00001692:"おばあさん",
    0x0000170B:"少年",
    0x00001784:"大雅",
    }

fs=open(r'Sakura.hcb','rb')
stm=bytefile.ByteFile(fs.read())
parser=hcbParser.HcbParser(stm)

parser.SetFuncNameTable(fnt,'932')
#txt=parser.Parse()
#fs=open('world1.txt','wb')
#fs.write(txt.encode('u16'))
#fs.close()
os.chdir('txt')
parser.ParseTxt([(3804,3836), (3984,4115)], 4449)
#parser.ParseTxt([], 4449)
