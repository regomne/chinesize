#encoding=utf-8
import hcbParser
import bytefile
import os

fnt={
    4:u'真紅',
    0x19a:u'加奈',
    0x3c5:u'加奈',
    0x3d5:u'澪',
    0x598:u'澪',
    0x5a8:u'鏡',
    0x6e8:u'つかさ',
    0x87e:u'藍',
    0x904:u'蓮',
    0xa17:u'白',
    0xb2a:u'鈴',
    0xd29:u'あゆむ',
    0xeec:u'時雨',
    0xfc4:u'とおる',
    0x114d:u'蓮也',
    0x1225:u'青空',
    0x13a0:u'さやか',
    0x14b3:u'静',
    0x1606:u'女１',
    0x164c:u'担任教師',
    0x16a2:u'女の子',
    0x171b:u'ハク',
    0x17f3:u'レン',
    0x18cb:u'看護師',
    0x1948:u'祖母',
    0x19e5:u'お母さん',
    0x1a5e:u'お母さん',
    0x1ad5:u'かみさま',
    0x1b82:u'男の人',
    0x1bfb:u'レン',
    0x1c66:u'ユウマ',
    0x1d99:u'悠馬',
    
    }

fs=open(ur'D:\Program Files\いろとりどりのヒカリ\hikari.hcb','rb')
stm=bytefile.ByteIO(fs.read())
parser=hcbParser.HcbParser(stm)

parser.SetFuncNameTable(fnt,'932')
#txt=parser.Parse()
#fs=open('world1.txt','wb')
#fs.write(txt.decode('932').encode('U16'))
#fs.close
os.chdir('txt')
parser.ParseTxt()
