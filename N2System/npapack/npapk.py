#encoding=utf-8
#py2.7

import os
import sys
import pdb
from struct import pack

key=[0xbd,0xaa,0xbc,0xb4,0xab,0xb6,0xbc,0xb4]

def decode(bf):
    for i in range(len(bf)):
        bf[i]^=key[i&7]

def packNpa(thedir,thefile):
    thedir=thedir.rstrip(os.sep)
    if os.sep in thedir:
        namepos=thedir.rfind(os.sep)+1
    else:
        namepos=0

    idxlen=4
    count=0
    for root, dirs, files in os.walk(thedir):
        for f in files:
            count+=1
            idxlen+=len((root[namepos:]+os.sep+f).encode('932'))+16

    fs=open(thefile,'wb')
    idx=bytearray()
    idx+=pack('I',count)
    curpos=idxlen+4
    fs.seek(curpos)
    for root, dirs, files in os.walk(thedir):
        for f in files:
            fname=(root[namepos:]+os.sep+f).encode('932')
            idx+=pack('I',len(fname))+fname.replace(os.sep,'/')
            flen=os.path.getsize(root+os.sep+f)
            idx+=pack('III',flen,curpos,0)
            curpos+=flen

            newf=open(root+os.sep+f,'rb')
            bf=bytearray(newf.read())
            decode(bf)
            fs.write(bf)
            newf.close()

    if len(idx)!=idxlen:
        pdb.set_trace()
    decode(idx)
    fs.seek(0)
    fs.write(pack('I',idxlen))
    fs.write(idx)
    fs.close()

#main
packNpa(ur'K:\Program Files\STEINSGATE\cc',ur'K:\Program Files\STEINSGATE\cc.npa')
