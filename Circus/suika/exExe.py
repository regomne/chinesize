import struct
import bytefile
from pdb import set_trace as int3

def GetStrs(stm,idx,txt,base):
    i=0
    int3()
    while i<len(stm):
        if stm[i]!=0:
            #print('\r%d'%len(txt))
            idx.append(i+base)
            stm.seek(i)
            txt.append(stm.readstr().decode('932'))
            i=stm.tell()
            continue
        i+=1

idx=[]
txt=[]
fs=open('Suika_ASPLUS_CN.exe','rb')
fs.seek(0x320d8)
stm=bytefile.ByteIO(fs.read(0x4141))
GetStrs(stm,idx,txt,0x320d8)

fs.seek(0x365f8)
stm=bytefile.ByteIO(fs.read(0x809))
GetStrs(stm,idx,txt,0x365f8)

fs.close()

fs=open('exe.txt','wb')
fs.write('\r\n'.join(txt).encode('U16'))
fs.close()

idxstm=bytearray()
for i in idx:
    idxstm+=struct.pack('I',i)

fs=open('exe.idx','wb')
fs.write(idxstm)
fs.close()
