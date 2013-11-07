#py3.2

import os
import bytefile

path1=r'F:\Program Files\majikoi\Config\\'
path2=r'F:\Program Files\majikoi\ConfigTxt\\'
path3=r'F:\Program Files\majikoi\NewConfig\\'

def packTxt(stm,lines):
    news=bytearray()
    count=stm.readu32()
    types=[stm.readu32() for i in range(count)]
    news+=stm[0:stm.tell()]
    
    j=0
    while stm.tell()<len(stm):
        for t in types:
            if t==2:
                news+=stm.read(4)
            elif t==1:
                l=stm.readstr()
                if len(l)!=0 and not l.endswith(b'.bin'):
                    news+=lines[j]+b'\0'
                    j+=1
                else:
                    news+=l+b'\0'
            else:
                int3()
    return news

dirs=os.listdir(path2)
for f in os.listdir(path1):
    if not f.endswith('.dat'): continue
    if f=='TDM45.dat': continue

    fs=open(path1+f,'rb')
    stm=bytefile.ByteIO(fs.read())
    fname=f.replace('.dat','.txt')
    if fname in dirs:
        fs=open(path2+f.replace('.dat','.txt'),'rb')
        lines=fs.read().decode('U16').encode('936','replace').split(b'\r\n')
        news=packTxt(stm,lines)
    else:
        news=stm[0:]
    fs=open(path3+f,'wb')
    fs.write(news)
