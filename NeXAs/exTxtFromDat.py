#py3.2

import os
import bytefile

path1=r'F:\Program Files\majikoi\Config\\'
path2=r'F:\Program Files\majikoi\ConfigTxt\\'

def exTxt(stm):
    count=stm.readu32()
    types=[stm.readu32() for i in range(count)]
    txt=[]
    while stm.tell()<len(stm):
        for t in types:
            if t==2:
                stm.readu32()
            elif t==1:
                l=stm.readstr()
                if len(l)!=0 and not l.endswith(b'.bin'):
                    txt.append(l)
            else:
                int3()
    return txt

for f in os.listdir(path1):
    if not f.endswith('.dat'): continue
    if f=='TDM45.dat': continue
    fs=open(path1+f,'rb')
    stm=bytefile.ByteIO(fs.read())
    lines=exTxt(stm)
    if len(lines)!=0:
        fs=open(path2+f.replace('.dat','.txt'),'wb')
        fs.write(b'\r\n'.join(lines).decode('936').encode('U16'))
