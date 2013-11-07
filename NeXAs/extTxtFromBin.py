#py3.2

import os
import bytefile

path1=r'Script\\'
path2=r'txt\\'

def exTxt(stm):
    inst_count=stm.readu32()
    stm.seek(inst_count*8+4)
    count=stm.readu32()
    txts=[]
    for i in range(count):
        txts.append(stm.readstr())
    return txts

for f in os.listdir(path1):
    fs=open(path1+f,'rb')
    stm=bytefile.ByteIO(fs.read())
    lines=exTxt(stm)
    fs=open(path2+f.replace('.bin','.txt'),'wb')
    fs.write(b'\r\n'.join(lines).decode('932').encode('U16'))
