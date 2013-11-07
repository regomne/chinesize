#encoding=utf-8
#py3.2

import os
import bytefile

key=[1,0xf1,0xee,0x8a]

def xordec(stm):
    i=0
    while(i<len(stm)):
        stm[i]^=key[i&3]
        i+=1

def ext(fname,path):
    if not os.path.exists(path):
        os.makedirs(path)
    fs=open(fname,'rb')
    hdr=bytefile.ByteIO(fs.read(0x8))
    xordec(hdr)
    count=hdr.readu32p(4)
    index=bytefile.ByteIO(fs.read(count*0x88))
    xordec(index)
    for i in range(count):
        name=index.read(0x80).rstrip(b'\0')
        size=index.readu32()
        off=index.readu32()
        fs.seek(off)
        content=bytearray(fs.read(size))
        xordec(content)
        newf=open(os.path.join(path,name.decode('sjis')),'wb')
        newf.write(content)
        newf.close()

ext(r'F:\Program Files\テンタクルロード\ten000_010.pkg',\
    r'F:\Program Files\テンタクルロード\ten000_010')
