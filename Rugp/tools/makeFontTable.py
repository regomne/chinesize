#encoding=utf-8
import struct

def GetFileChars(stm,chars):
    for c in stm:
        if ord(c)<0x100:
            continue
        if c not in chars:
            chars.append(c)

def c2i(c):
    s=0
    for b in c:
        s=(s<<8)+b
    return s

def FillSjisTable(chars,fixedChars):
    tbl=[0 for i in range(65536)]
    for c in fixedChars:
        tbl[c2i(c.encode('932'))]=c2i(c.encode('936'))

    i=0x8140
    j=0
    while j<len(chars):
        ih=(i>>8)
        il=i&0xff
        if ih==0xa0:
            i=0xe000+il
        if ih==0xfc:
            raise Exception("超出范围")
        if il<0x40:
            i=(ih<<8)+0x40
        if il==0x7f:
            i=(ih<<8)+0x80
        if il==0xfc:
            i=((ih+1)<<8)+0x40

        if tbl[i]!=0:
            i+=1
            continue
        c=chars[j]
        if c in fixedChars:
            j+=1
            continue
        tbl[i]=c2i(c.encode('936'))
        j+=1
        i+=1
    return tbl

def makeReverseTbl(tbl):
    ntbl=[0 for i in range(len(tbl))]
    for i in range(len(tbl)):
        if(tbl[i]!=0):
            ntbl[tbl[i]]=i
    return ntbl

def writeTbl(fs,tbl,le):
    if not le:
        for v in tbl:
            fs.write(struct.pack('>H',v))
    else:
        for v in tbl:
            fs.write(struct.pack('<H',v))
            
fixed='【武】純夏声'
chars=[]
GetFileChars(open(r'd:\temp\02.txt','rb').read().decode('936'),chars)
tbl=FillSjisTable(chars,fixed)
rvtbl=makeReverseTbl(tbl)
writeTbl(open('map.tbl','wb'),tbl,True)
writeTbl(open('rvtbl.tbl','wb'),rvtbl,False)
