#py3.2

import zlib
import bytefile
import os

path1=r'D:\chinesize\NeXAs\Script\\'
path2=r'D:\chinesize\NeXAs\Scriptt\\'
path3=r'D:\chinesize\NeXAs\nScript\\'

def packTxtBlock(lines):
    block=bytearray()
    for l in lines:
        block+=l+b'\x00'
    return block

def packTxt(stm,lines):
    #stm.seek(4)
    inst_count=stm.readu32()
    stm.seek(inst_count*8+4)
    count=stm.readu32()
    pos1=stm.tell()
    for i in range(count):
        stm.readstr()
    pos2=stm.tell()
    newstm=bytearray()
    newstm+=stm[0:pos1]
    newstm+=packTxtBlock(lines)
    newstm+=stm[pos2:]
    return newstm

for f in os.listdir(path1):
    fs=open(path1+f,'rb')
    stm=bytefile.ByteIO(fs.read())
    if os.path.exists(path2+f.replace('._bin','.txt')):
        fs=open(path2+f.replace('._bin','.txt'),'rb')
        lines=fs.read().decode('U16').encode('936','replace').split(b'\r\n')
        stm=packTxt(stm,lines)
    fs=open(path3+f.replace('._bin','.bin'),'wb')
    fs.write(stm[0:])
