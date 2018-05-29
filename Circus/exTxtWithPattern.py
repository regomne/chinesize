import bytefile
import re
import struct

patSel=re.compile(rb'D\d\d\0')
SEL_TEXT_OP=0x4F

def getSelText(stm):
    pos=0
    txt=[]
    offs=[]
    while True:
        mo=patSel.search(stm,pos)
        if mo:
            textPos=mo.end(0)
            if stm[textPos]!=SEL_TEXT_OP:
                raise Exception('sel op error in pos:%x'%textPos)
            textEnd=stm.find(0,textPos)
            if textEnd==-1:
                raise Exception('sel text no 0 in pos:%x'%textPos)
            nl=[n+0x20 for n in stm[textPos+1:textEnd]]
            txt.append(bytes(nl))
            offs.append(textPos+1)
            pos=textEnd
        else:
            break
    return txt,offs

def getIndex(stm):
    bs=bytefile.ByteFile(stm)
    indexCnt=bs.readu32()
    index=[]
    for i in range(indexCnt):
        index.append(bs.readu32())
    return index

def fixIndex(index, off, dist):
    for i in range(len(index)):
        idx=index[i]
        highBit=idx&0x80000000
        idx&=0x7fffffff
        if idx>off:
            index[i]=(idx+dist)|highBit
    return index

def packSel(fname, txtname, newname):
    fs=open(fname,'rb')
    stm=fs.read()
    fs.close()
    _,offs=getSelText(stm)
    index=getIndex(stm)
    fs=open(txtname,'rb')
    ls=fs.read().decode('utf-8-sig').split('\r\n')
    fs.close()
    if len(ls)<len(offs):
        raise Exception('txt lines error')
    for i in range(len(ls)):
        l=ls[i].encode('936')
        l=[n-0x20 for n in l]
        ls[i]=bytes(l)
    ns=bytearray(stm)
    for i in range(len(offs)):
        oriStart=offs[i]
        oriEnd=ns.find(0,oriStart)
        oriLen=oriEnd-oriStart
        ns[oriStart:oriEnd]=ls[i]
        dist=len(ls[i])-oriLen
        if dist!=0:
            k=i+1
            while k<len(offs):
                offs[k]+=dist
                k+=1
            off=oriStart-len(index)*4-4
            fixIndex(index, off, dist)
    newIndex=bytearray()
    for idx in index:
        newIndex+=struct.pack('<I',idx)
    fs=open(newname,'wb')
    fs.write(stm[0:4])
    fs.write(newIndex)
    fs.write(ns[len(index)*4+4:])
    fs.close()

def extSel(fname, txtname):
    fs=open(fname,'rb')
    stm=fs.read()
    #bs=bytefile.ByteFile(fs.read())
    fs.close()
    txts,_=getSelText(stm)
    txts=[l.decode('932') for l in txts]
    fs=open(txtname,'wb')
    fs.write('\r\n'.join(txts).encode('utf-8-sig'))
    fs.close()
    
#extSel('f02_06_10.mes','f02_06_101.txt')
#packSel('f02_04_10.mes','f02_04_10.txt','1f02_04_10.mes')
packSel('f02_06_10.mes','f02_06_10.txt','1f02_06_10.mes')
