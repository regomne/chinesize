import os
import struct

def packdir(path,fname):
    files=os.listdir(path)
    files.sort()
    base_size=8
    for f in files:
        base_size+=8+(len(f)+1)*2
    fs=open(fname,'wb')
    fs.seek(base_size)
    idxes=[]
    curIdx=0
    for f in files:
        nf=open(os.path.join(path,f),'rb')
        stm=nf.read()
        fs.write(stm)
        nf.close()
        bs=struct.pack('<II',len(stm),curIdx)
        curIdx+=len(stm)
        idxes.append(bs+f.encode('utf-16-le')+b'\0\0')
    idxbin=b''.join(idxes)
    if len(idxbin)!=base_size-8:
        raise Exception('idx size error')
    fs.seek(0)
    fs.write(struct.pack('<II',len(files),base_size-8))
    fs.write(idxbin)
    fs.close()

packdir(r'd:\Game\見上げてごらん、夜空の星を Interstellar Focus\Script','Script.arc')
