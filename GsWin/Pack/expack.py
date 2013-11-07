#encoding=utf-8
#py3.2

from struct import unpack
import io
import os

from pdb import set_trace as int3

PakName='scr.pak'
PathName='scr'

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

def ExtPak(fs,dirname):
    fs.seek(0)
    magic, desc, minor_ver, major_ver, index_len, flags, index_entries,\
           data_offset, index_offset=unpack('16s32sHHIIIII',fs.read(72))

    if(magic.rstrip(b'\0')!=b'DataPack5'):
        print("Format Error!")
        return 0
    fs.seek(index_offset)
    idx=bytearray(fs.read(index_len))
    if(flags&1):
        Decrypt(idx)
    idx=LzssUnc((bytes)(idx),index_entries*0x68)
    #int3()
    idxstm=io.BytesIO(idx)
    for i in range(index_entries):
        fname,offset,length=unpack('64sII32x',idxstm.read(0x68))
        fname=fname.rstrip(b'\0').decode('932')
        if(fname==''):
            xxx=0
        newf=open(os.path.join(dirname,fname),'wb')
        fs.seek(offset+data_offset)
        buff=fs.read(length)
        newf.write(buff)
        newf.close()

fs=open(PakName,'rb')
ExtPak(fs,PathName)

