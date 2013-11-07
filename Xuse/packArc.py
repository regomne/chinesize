#encoding=utf-8
#py2.7
import strfile
from struct import unpack,pack
from pdb import set_trace as int3
import os

def toint(str1):
    return (ord(str1[3])<<24)+(ord(str1[2])<<16)+(ord(str1[1])<<8)+(ord(str1[0]))
def toshort(str1):
    return (ord(str1[1])<<8)+(ord(str1[0]))

def decStr(s):
    name=[]
    for ch in s:
        name.append(chr(ord(ch)^0x56))
    return ''.join(name)

filename='rio.arc'
arcpath='rio\\'

fs=open(filename,'rb+')
fs.seek(0x10)
count=toint(fs.read(4))
fs.seek(0x1a)
cadr_off=toint(fs.read(4))+4
fs.seek(0)
stm=strfile.MyStr(fs.read(cadr_off+count*12))

stm.seek(0x28+2)
for i in range(count):
    cur=stm.tell()
    stm.seek(stm.readu32()+4)
    idx, len1, crc, =unpack('HHH',stm.read(6))
    fname=decStr(stm.read(len1)).decode('932')
    data_off,=unpack('Q',stm[cadr_off+i*12+2:cadr_off+i*12+10])
    fs.seek(data_off)
    magic, data1, length, =unpack('4s12sI',fs.read(20))
    if magic!='DATA': int3()
    newf=open(arcpath+fname,'rb')
    newf.seek(0,2)
    if newf.tell()<=length:
        newl=pack('I',newf.tell())
        fs.seek(-4,1)
        fs.write(newl)
        fs.seek(6,1)
        newf.seek(0)
        fs.write(newf.read())
    else:
        fs.seek(0,2)
        fend=fs.tell()
        newl=pack('I',newf.tell())
        fs.write(magic+data1+newl+'\x00\x00')
        newf.seek(0)
        fs.write(newf.read())
        fs.seek(cadr_off+i*12+2)
        newo=pack('Q',fend)
        fs.write(newo)
    newf.close()
    stm.seek(cur+8)
