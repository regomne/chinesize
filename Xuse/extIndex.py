#encoding=utf-8
#py2.7
import strfile
from struct import unpack
import ctypes

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

fs=open(filename,'rb')
fs.seek(0x10)
count=toint(fs.read(4))
fs.seek(0x1a)
cadr_off=toint(fs.read(4))
fs.seek(0)
stm=strfile.MyStr(fs.read(cadr_off+count*12))
fs.close()

ctif_off=toint(stm[0x2A:0x2A+4])

names=[]
stm.seek(0x28+2)
for i in range(count):
    cur=stm.tell()
    stm.seek(stm.readu32()+4)
    idx, len1, crc, =unpack('HHH',stm.read(6))
    names.append(('0x%02X\t'%idx)+decStr(stm.read(len1)))
    stm.seek(cur+8)
txt='\r\n'.join(names)

fs=open(filename.replace('.arc','_idx.txt'),'wb')
fs.write(txt.decode('932').encode('utf-16'))
fs.close()
