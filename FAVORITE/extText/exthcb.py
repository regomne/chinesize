#encoding=utf-8
import hcbParser
import bytefile
import os

extOriginalScript = False

fnt={
    0x00000004:"メア",
    0x000000CB:"メア",
    0x000000DC:"明日歩",
    0x0000017F:"こもも",
    0x00000222:"こさめ",
    0x000002C5:"衣鈴",
    0x00000344:"千波",
    0x000003C3:"雪菜",
    0x00000442:"夢",
    0x000004E5:"詩乃",
    0x00000564:"万夜花",
    0x000005E3:"鈴葉",
    0x00000662:"飛鳥",
    0x000006E1:"岡泉先輩",
    0x00000760:"総一朗",
    0x00000803:"かーくん",
    0x00000882:"レン",
    0x00000949:"大河",
    0x000009EC:"千尋",
    0x00000A55:"伊麻",
    0x00000ABE:"歌澄",
    0x00000B27:"看護師",
    0x00000BA6:"女１",
    0x00000BE2:"男１",
    0x00000C1E:"洋",
    }

fs=open(r'Sakura.hcb','rb')
stm=bytefile.ByteFile(fs.read())
parser=hcbParser.HcbParser(stm)

if extOriginalScript:
    txt=parser.Parse()
    fs=open('world1.txt','wb')
    fs.write(txt.encode('u16'))
    fs.close()
else:
    parser.SetFuncNameTable(fnt,'932')
    os.chdir('txt')
    parser.ParseTxt([(4273, 4292), (4434, 4535)], 4860)
