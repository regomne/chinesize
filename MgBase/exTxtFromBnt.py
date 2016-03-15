import bytefile
import struct
from pdb import set_trace as int3

Txt=[]

def GetVal(stm):
    t=stm.readu32()
    if t==0x8000010:
        l=stm.readu32()
        s=stm.read(l)
        half=True
        for c in s:
            if c>0x80:
                half=False
                break
        if not half:
            Txt.append(s.decode('932'))
        return s
    elif t==0x5000002:
        return stm.readu32()
    elif t==0x5000004:
        return stm.readu32()
    elif t==0x1000001:
        return 0
        
def CheckMagic(stm):
    if stm.readu32()!=0x50415254:
        int3()

def GetPart(stm):
    CheckMagic(stm)
    src_name=GetVal(stm)
    func_name=GetVal(stm)

    CheckMagic(stm)
    counts=struct.unpack('I'*8,stm.read(32))

    CheckMagic(stm)
    for i in range(counts[0]):
        GetVal(stm)

    CheckMagic(stm)
    for i in range(counts[1]):
        GetVal(stm)

    CheckMagic(stm)
    for i in range(counts[2]):
        stm.seek(4,1)
        GetVal(stm)
        GetVal(stm)

    CheckMagic(stm)
    for i in range(counts[3]):
        GetVal(stm)
        stm.seek(12,1)

    CheckMagic(stm)
    stm.seek(8*counts[4],1)

    CheckMagic(stm)
    stm.seek(4*counts[5],1)

    CheckMagic(stm)
    stm.seek(8*counts[6],1)

    CheckMagic(stm)
    for i in range(counts[7]):
        GetPart(stm)

    stm.seek(6,1)

def ExtTxt(stm):
    if stm.read(2)!=b'\xfa\xfa':
        int3()
    if stm.readu32()!=0x53514952:
        int3()
    if stm.readu32()!=1:
        int3()
    GetPart(stm)
    if stm.read(4)!=b'LIAT':
        int3()

fs=open('ms010.bnt','rb')
stm=bytefile.ByteIO(fs.read())
fs.close()
Txt=[]
ExtTxt(stm)
