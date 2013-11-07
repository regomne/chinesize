import struct
import bytefile
from pdb import set_trace as int3

def GetStrs(stm,idx,txt,base):
    i=0
    #int3()
    while i<len(stm):
        if stm[i]!=0:
            #print('\r%d'%len(txt))
            idx.append(i+base)
            stm.seek(i)
            txt.append(stm.readstr())
            i=stm.tell()
            continue
        i+=1

idx=[]
txt=[]
fs=open('SAKURA_CHS2.EXE','rb')
fs.seek(0x34648)
stm=bytefile.ByteIO(fs.read(0x3a800))
GetStrs(stm,idx,txt,0x34648)

fs.close()

fs=open('exe.txt','wb')
fs.write(b'\r\n'.join(txt))
fs.close()

idxstm=bytearray()
for i in idx:
    idxstm+=struct.pack('I',i)

fs=open('exe.idx','wb')
fs.write(idxstm)
fs.close()
