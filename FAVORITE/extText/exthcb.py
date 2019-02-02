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
    0x00000D3A:"サブ２",
    0x00000E14:"サブ３",
    0x00000F2F:"サブ４",
    0x0000104A:"サブ５",
    0x00001188:"サブ６",
    0x0000128B:"サブ７",
    0x0000139E:"母",
    0x00001417:"女の人",
    0x00001490:"女の人",
    0x00001509:"幼女",
    0x00001582:"母親",
    0x000015FB:"猫",
    0x00001674:"蝶ネクタイをしたすまし顔の猫",
    0x0000170F:"おばあさん",
    0x00001788:"少年",
    0x00001801:"夜のイキモノ",
    0x0000187C:"奏大雅",
    0x00001918:"大雅",
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
parser.ParseTxt([(4783, 4805), (4953, 5083)], 5439)
