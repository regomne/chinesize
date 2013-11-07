#py3.2

import os
from struct import unpack

scrpath='scr'

def LzssUnc(buff,outlen):
    uncompr=bytearray(outlen)
    win=bytearray(0x1000)
    iWin=0xfee
    iSrc=0
    iDest=0
    while iSrc<len(buff):
        code=buff[iSrc]
        iSrc+=1
        for i in range(8):
            if (code>>i) & 1:
                if iDest>=outlen: break
                uncompr[iDest]=buff[iSrc]
                win[iWin]=buff[iSrc]
                iSrc+=1
                iDest+=1
                iWin=(iWin+1)&0xfff
            else:
                if iSrc+1>=len(buff): break
                count=(buff[iSrc+1] & 0xF)+3
                pos=(buff[iSrc]) | ((buff[iSrc+1]&0xf0)<<4)
                iSrc+=2
                for j in range(count):
                    d=win[(pos+j)&0xfff]
                    if iDest>=outlen: return uncompr
                    uncompr[iDest]=d
                    win[iWin]=d
                    iDest+=1
                    iWin=(iWin+1)&0xfff
    return uncompr

def Decrypt(stm):
    for i in range(len(stm)):
        stm[i]^=i&0xff

for f in os.listdir(scrpath):
    fs=open(os.path.join(scrpath,f),'rb+')
    magic,is_compr,unclen,comprlen,inst_count,str_count,inst_len,\
        str_len=unpack('16s4xiII4xII4xII4x',fs.read(0x3c))
    fs.seek(0x1c8)
    if(is_compr==-1):
        buff=bytearray(fs.read(comprlen))
        Decrypt(buff)
        buff=LzssUnc((bytes)(buff),unclen)
    else:
        buff=bytearray(fs.read(unclen))
        Decrypt(buff)
    fs.seek(0x1c8)
    fs.write(buff)
    fs.close()

