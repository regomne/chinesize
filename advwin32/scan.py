#encoding=utf-8
#py3.2

import struct
import bytefile
import os
from pdb import set_trace as int3

def GetBracket(mes):
    op=mes.readu8()
    ctnt=[]
    while op!=1:
        if op==0xe2:
            ctnt.append(mes.readu8())
        elif op==0xe3:
            ctnt.append(mes.readu16())
        elif op==0xe4:
            val=mes.readu32()
            ctnt.append(val)
        elif op==0xe5:
            ctnt.append(mes.readstr())
        elif (op>=0xe6) and (op<=0xe8):
            ctnt.append(mes.readu16())
        elif (op>=0xb0) and (op<=0xd5):
            ctnt.append(op*-1)
        else:
            print('%x'%mes.tell())
            int3()
        op=mes.readu8()
    return ctnt

def GetPara(mes,op):
    if (op==4) or (op==5):
        return -1
    elif ((op>=0x13) and (op<=0x14)) or (op==0xe1):
        return GetBracket(mes)
    elif op==0xe2:
        return mes.readu8()
    elif op==0xe3:
        return mes.readu16()
    elif op==0xe4:
        val=mes.readu32()
        return val
    elif op==0xe5:
        return mes.readstr()
    elif (op>=0xe6) and (op<=0xe8):
        return mes.readu16()
    else:
        return -op

def GetOp(mes):
    op=mes.readu8()
    while op==0x12:
        op=mes.readu8()
    if op==0:
        print('%x'%mes.tell())
        int3()
    if op==2:
        return (2,0)
    if (op==3) or (op==4):
        n=mes.readu8()
        if n!=0xe4:
            print('%x'%mes.tell())
            int3()
        return (op,mes.readu32())
    if op>0x7b:
        print('%x'%mes.tell())
        int3()
        return -1
    if (op==0x13) or (op==0x14):
        return (op,GetBracket(mes))
    if (op==0x11):
        return (op,mes.readstr())
    if op==0x10:
        print('%x'%mes.tell())
        int3()
    if (op<4) or ((op>5) and (op<0x15)) or (op>0x7b):
        return (op,0)
    if op<=5:
        return (op,0)
    nextop=mes.readu8()
    if nextop<=3:
        mes.seek(-1,1)
        return (op,0)
    paras=[]
    while nextop>3:
        if (nextop>=0x10) and (nextop<=0x7b):
            break
        para=GetPara(mes,nextop)
        if para==-1:
            break;
        paras.append(para)
        nextop=mes.readu8()
    mes.seek(-1,1)
    return (op,paras)

def GetStr(obj,text):
    if (type(obj)==tuple) or (type(obj)==list):
        for el in obj:
            GetStr(el,text)
    else:
        if (type(obj)==bytearray) or (type(obj)=='bytes'):
            text.append(obj.decode('932'))
    

def ScanMes(mes):
    text=[]
    magic=mes.read(0x10)
    if magic!=b'ADVWin32 1.00  \0':
        print('format error.')
        print(magic)
        return 0
    fsize,hdrsize=struct.unpack('4xII4x',mes.read(0x10))

    mes.seek(hdrsize)
    while mes.tell()<fsize-hdrsize:
        opg=GetOp(mes)
        #GetStr(opg,text)
    return text

path1=r'F:\Program Files\c2_dvd1\game01'
path2=r'F:\Program Files\c2_dvd1\game01t'

for f in os.listdir(path1):
    fs=open(os.path.join(path1,f),'rb')
    stm=bytefile.ByteIO(fs.read())
    fs.close()
    print(f+' scanning...')
    txt=ScanMes(stm)
##    fs=open(os.path.join(path2,f.rpartition('.')[0]+'.txt'),'wb')
##    fs.write('\r\n'.join(txt).encode('u16'))
##    fs.close()
