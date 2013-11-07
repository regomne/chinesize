import struct
import os
import sys

def writeMbLong(val):
    val&=0xffffffff
    b=[]
    if val==0:
        return bytes([1])
    while val>0:
        b.insert(0,((val&0x7f)<<1))
        val>>=7
    #b.insert(0,(val&0x7f)<<1)
    b[-1]|=1
    return bytes(b)

if len(sys.argv)!=3:
    print('usage: %s dirname iganame'%sys.argv[0])
    sys.exit()

path=sys.argv[1]

fileList=[]
off=0
nameoff=0
namesTbl=bytearray()

print('collecting names...')
for f in os.listdir(path):
    if os.path.isdir(os.path.join(path,f)):
        continue
    fsize=os.stat(os.path.join(path,f)).st_size
    fileList.append((f,off,nameoff,fsize))
    off+=fsize
    bname=f.encode('932')
    for b in bname:
        namesTbl+=writeMbLong(b)
    nameoff=len(namesTbl)

hdr=b'IGA0\0\0\0\0\x02\0\0\0\x02\0\0\0'

print('writing index...')
idx=bytearray()
for name,off,nameoff,size in fileList:
    idx+=writeMbLong(nameoff)+writeMbLong(off)+writeMbLong(size)

#newIgaName='script1.iga'
newIgaName=sys.argv[2]

print('writing files')
fs=open(newIgaName,'wb')
fs.write(hdr)
fs.write(writeMbLong(len(idx)))
fs.write(idx)
fs.write(writeMbLong(len(namesTbl)))
fs.write(namesTbl)

for name,off,nameoff,size in fileList:
    fs1=open(os.path.join(path,name),'rb')
    stm=bytearray(fs1.read())
    if len(stm)!=size:
        asdgfrewt
    for i in range(len(stm)):
        stm[i]^=((i+2)&0xff)
    fs.write(stm)
    fs1.close()

fs.close()

print('complete')
input()
