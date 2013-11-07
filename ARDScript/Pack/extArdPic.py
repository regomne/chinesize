#encoding=utf-8
#py3.2

import os
import zlib
import bytefile
from pdb import set_trace as int3

dec_key=b'MaidLove'

def DecSingle(stm):
    for i in range(len(stm)):
        stm[i]^=dec_key[i&7]
    int3()
    return zlib.decompress(stm[4:])
def ExtSingle(stm):
    return zlib.decompress(stm[4:])

path='E:\\Game\\夏之灯火\\'
fs=open(path+'ARDPics.ARD','rb')
hdr=bytefile.ByteIO(fs.read(0x100))
hdr.seek(8)
count=hdr.readu32()
idx=bytefile.ByteIO(fs.read(count*0x2d))

if 'ARDPics' not in os.listdir(path):
    os.mkdir(path+'ARDPics')
os.chdir(path+'ARDPics')

for i in range(count):
    off=idx.readu32()
    size=idx.readu32()
    resv=idx.readu32()
    if size!=resv:
        aaa
    fs.seek(off)
    stm=bytearray(fs.read(size))
    name=idx.read(0x21).rstrip(b'\0').decode('932')
    if name.lower().endswith('.piz'):
        nf=open(name+'.bmp','wb')
        nf.write(ExtSingle(stm))
        nf.close()
    elif name.lower().endswith('.psf'):
        nf=open(name+'.txt','wb')
        stm=stm.decode('932').encode('U16')
        nf.write(stm)
        nf.close()
    elif name.lower().endswith('.snf'):
        nf=open(name+'.txt','wb')
        stm=DecSingle(stm).decode('932').encode('936')
        nf.write(stm)
        nf.close()
    else:
        nf=open(name,'wb')
        nf.write(stm)
        nf.close()
        print('!!!!')
    print(name)
