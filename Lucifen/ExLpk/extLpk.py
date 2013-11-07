#encoding=utf-8
#py3.2

import bytefile
import lpkkey
import struct
import os
from pdb import set_trace as int3

'''lpkcrypt struct
0 use_short_off
1 use_tree_files
2 is_crypted
3 is_compressed
4 total_crypted
5 key
6 plaindata

MyEntry struct
0 name
1 offset
2 is_crypted
3 comprlen
4 uncomprlen
'''

ci=[0,0,0,0,0,0,0]
def addentry(name,idxes,entrylist,np):
    global ci
    ent=[0,0,0,0,0]
    if ci[3]:
        entrylist.seek(np*13)
    else:
        entrylist.seek(np*9)
    ent[0]=name
    ent[2]=entrylist.readu8()
    if ci[0]:
        ent[1]=(entrylist.readu32())<<11
    else:
        ent[1]=(entrylist.readu32())
    ent[3]=entrylist.readu32()
    if ci[3]:
        ent[4]=entrylist.readu32()
    idxes[np]=ent

def longidxsrch(bf,name,depth,idxes,entrylist):
    subnum=bf.readu8()
    node=bf.tell()
    for i in range(subnum):
        c=bf.read(1)
        np=bf.readu32()
        if c[0]!=0:
            name+=c
            bf.seek(np,1)
            longidxsrch(bf,name,depth+1,idxes,entrylist)
            name=name[0:depth]
        else:
            addentry(name,idxes,entrylist,np)
        node+=5
        bf.seek(node)

def shortidxsrch(bf,name,depth,idxes,entrylist):
    subnum=bf.readu8()
    node=bf.tell()
    for i in range(subnum):
        c=bf.read(1)
        np=bf.readu16()
        if c[0]!=0:
            name+=c
            bf.seek(np,1)
            shortidxsrch(bf,name,depth+1,idxes,entrylist)
            name=name[0:depth]
        else:
            addentry(name,idxes,entrylist,np)
        node+=3
        bf.seek(node)

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

def DecHdr(fs,name,keys):
    fs.seek(4)
    idxlen,=struct.unpack('I',fs.read(4))
    key0=0xa5b9ac6b
    key1=0x9a639de5
    name=name.upper().encode('932')
    decname=name.split(b'.')[0]
    for i in range(len(decname)):
        key0^=decname[-i-1]
        key1^=decname[i]
        key0=(key0>>7) | ((key0<<25)&0xffffffff)
        key1=(key1>>25) | ((key1<<7)&0xffffffff)

    key0^=keys[name][0]
    key1^=keys[name][1]
    idxlen^=key1
    flags=idxlen>>24
    idxlen&=0xffffff

    if flags&1:
        idxlen=(idxlen<<11)-8

    global ci
    ci[0]=(flags>>0)&1
    ci[1]=(flags>>1)&1
    ci[2]=(flags>>2)&1
    ci[3]=(flags>>3)&1
    ci[4]=(flags>>4)&1
    ci[5]=key0
    return fs.read(idxlen),key0,key1

def ExtDir(idx,key0,key1):
    idx=bytefile.ByteIO(idx)
    d=0x31746285
    for i in range(int(len(idx)/4)):
        d=((d<<4)&0xffffffff) | (d>>28)
        shift=d&0x1f
        tempkey=[key1&0xff,(key1&0xff00)>>8,(key1&0xff0000)>>16,(key1&0xff000000)>>24]
        for j in range(4):
            idx[i*4+j]^=tempkey[j]
        key1=(key1>>shift)|((key1<<(32-shift))&0xffffffff)

    idx.seek(0)
    entry_num=idx.readu32()

    global ci
    predata_len=idx.readu8()
    ci[6]=idx.read(predata_len)
    dirs=[0 for i in range(entry_num)]
    if ci[1]:
        mode=idx.readu8()
        nametree_len=idx.readu32()
        tree=bytefile.ByteIO(idx.read(nametree_len))
        entrylist=bytefile.ByteIO(idx[idx.tell():])
        name=b''
        if mode:
            longidxsrch(tree,name,0,dirs,entrylist)
        else:
            shortidxsrch(tree,name,0,dirs,entrylist)

    return dirs

def ExtSingleFile(ent,pkg):
    global ci
    pkg.seek(ent[1])
    dest=bytearray(pkg.read(ent[3]-len(ci[6])))
    if ent[4]:
        dest=bytearray(LzssUnc(dest,ent[4]))
    if ci[2]:
        if ci[4]:
            for i in range(len(dest)):
                dest[i]=((dest[i]^0x50)>>4)+(((dest[i] ^ 0xFD) & 0xF) << 4)
        dec_len=int(len(dest)/4)
        if dec_len>64:
            dec_len=64
        key1=ci[5]
        d=0x31746285
        for i in range(dec_len):
            d=((d<<28)&0xffffffff) | (d>>4)
            shift=d&0x1f
            tempkey=[key1&0xff,(key1&0xff00)>>8,(key1&0xff0000)>>16,(key1&0xff000000)>>24]
            for j in range(4):
                dest[i*4+j]^=tempkey[j]
            key1=(key1>>(32-shift))|((key1<<shift)&0xffffffff)
    return ci[6]+dest

fname='sys.lpk'
fs=open(fname,'rb')
int3()
idx,key0,key1=DecHdr(fs,fname.rsplit('\\',1)[-1],lpkkey.key)
dirs=ExtDir(idx,key0,key1)
dirname=fname.rsplit('\\',1)[-1].rsplit('.',1)[0]
if dirname not in os.listdir('.'):
    os.mkdir(dirname)
os.chdir(dirname)
for fe in dirs:
    fc=ExtSingleFile(fe,fs)
    newf=open(fe[0],'wb')
    newf.write(fc)
    newf.close()

