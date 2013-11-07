#py2.7
import os
import zlib
from struct import pack

dec_key=[0x4D, 0x61, 0x69, 0x64, 0x4C, 0x6F, 0x76, 0x65]#'MaidLove'

def packSingle(stm):
    compr=zlib.compress(stm)
    dest=bytearray(pack('I',len(compr))+compr)
    for i in range(len(dest)):
        dest[i]^=dec_key[i&7]
    return dest

path='new3'

count=0
for f in os.listdir(path):
    if f.endswith('.txt'):
        count+=1

fs=open(path+'.ard','wb')
curoff=count*0x2d+0x100
fs.seek(curoff)

idx=[]
for f in os.listdir(path):
    if not f.endswith('.txt'): continue
    newf=open(path+os.sep+f,'rb')
    stm=newf.read().decode('U16').encode('936','replace')
    newf.close()
    news=packSingle(stm)
    entry=pack('III',curoff,len(news),len(news))
    entry+=f[0:-4].encode('932').ljust(0x21,'\0')
    curoff+=len(news)
    idx.append(entry)
    fs.write(news)

hdr='\0'*4+'ARD0'+pack('I',count)+'\0'*0xf4
hdr+=''.join(idx)
fs.seek(0)
fs.write(hdr)
fs.close()
