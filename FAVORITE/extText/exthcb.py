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
    0x1260:u'青空',
    0x13db:u'さやか',
    0x14ee:u'静',
    0x1641:u'苺',
    0x1719:u'女１',
    0x175f:u'担任教師',
    0x17b5:u'女の子',
    0x182e:u'ハク',
    0x1906:u'レン',
    0x19de:u'看護師',
    0x1a5b:u'祖母',
    0x1af8:u'お母さん',
    0x1b71:u'お母さん',
    0x1be8:u'かみさま',
    0x1c95:u'男の人',
    0x1d0e:u'レン',
    0x1d79:u'ユウマ',
    0x1eac:u'悠馬',
    
    }

fs=open(ur'shinku.hcb','rb')
stm=bytefile.ByteIO(fs.read())
parser=hcbParser.HcbParser(stm)



parser.SetFuncNameTable(fnt,'932')
#txt=parser.Parse()
#fs=open('world1.txt','wb')
#fs.write(txt.decode('932').encode('U16'))
#fs.close()
os.chdir('txt')
parser.ParseTxt([(1439,4423),(4484,4770)],5042)
