#encoding=utf-8
import ACTree
import struct
import random
import time
import os

##rpath=r'D:\Program Files\マブラヴ14'
##paths=['txt','r6ti','r7','r08']
rpath=r'F:\Download\处理'
paths=['txt','r6ti']


def CalcHash(s):
    h=0
    for c in s:
        h+=(h<<5)+c
    return h&0xffffffff

def Encrypt(stm,key):
    ns=bytearray(stm)
    i=1
    while i<len(ns):
        ns[i]^=ns[i-1]^key[i&3]
        i+=1
    return ns

def GenerateRandomNums():
    random.seed(time.clock())
    strs=[]
    count=0
    for pa in paths:
        for f in os.listdir(os.path.join(rpath,pa)):
            strs.append(os.path.join(pa,f).lower().encode('936')+b'\0')
            count+=1
    print(count,' files')
    IDs=[i+1 for i in range(count)]
    i=0
    while i<count:
        r=random.randrange(count)
        t=IDs[i]
        IDs[i]=IDs[r]
        IDs[r]=t
        i+=1
    
    return (strs,IDs)

def Pack(fs,strs,IDs,ft,rvt):

    ftStm=bytearray()
    for c in ft:
        ftStm+=struct.pack('h',c)
    rvtStm=bytearray()
    for c in rvt:
        rvtStm+=struct.pack('h',c)
    
    magic=0x21857507
    treeSize=len(ftStm)
    if len(ftStm)!=len(rvtStm):
        raise Exception("error")
    treeOff=40
    vtreeOff=treeOff+treeSize
    entryOff=vtreeOff+treeSize
    entrySize=16*(len(strs)+1)
    leafSize=2
    leafVSize=2
    hdrstm=struct.pack('<IQIQQIBB',magic,treeOff,treeSize,vtreeOff,
                       entryOff,entrySize,leafSize,leafVSize).ljust(40)

    fs.seek(0)
    fs.write(hdrstm)
    fs.write(ftStm)
    fs.write(rvtStm)
    
    off=0x40+treeSize*2+entrySize
    fs.seek(off)

    sss=[(IDs[i],strs[i]) for i in range(len(strs))]
    sss.sort()
    entries=[[0,0]]*(len(sss)+1)
    for num,nameb in sss:
        h=CalcHash(nameb)
        key=[h&0xff,(h>>8)&0xff,(h>>16)&0xff,(h>>24)&0xff]
        nfs=open(os.path.join(rpath,nameb[0:-1].decode('936')),'rb')
        stm=Encrypt(nfs.read(),key)
        nfs.close()
        fs.write(stm)
        entries[num]=[off^h,len(stm)^(h<<2)]
        off+=len(stm)

    entrystm=bytearray()
    for off1,length in entries:
        entrystm+=struct.pack('QQ',off1,length)
    fs.seek(entryOff)
    fs.write(entrystm)
    #fs.close()

strs,IDs=GenerateRandomNums()


tree=ACTree.MakeACTree(strs,IDs)
ft,rvt=ACTree.FlattenTree(tree)

print("packing...")
fs=open('cnpack','wb')
Pack(fs,strs,IDs,ft,rvt)
fs.close()
