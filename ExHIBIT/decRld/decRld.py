#encoding=utf-8
#py3.2

import os
import bytefile

path1=r'd:\Game\家の恋人\rld'
path2=r'd:\Game\家の恋人\rld2'
GameName='家の恋人'

def DecRld(stm, key1, key2):
    if len(stm)-0x10>0x3ff0*4:
        declen=0x3ff0*4
    else:
        declen=(len(stm)-0x10)&0xfffffffc
    for i in range(declen):
        stm[i+0x10] ^= key1[i&0x3ff] ^ key2[i&3]


def GetKey(gamename):
    fs=open(gamename+'key1.dat','rb')
    key1=fs.read()
    fs.close()
    fs=open(gamename+'key2.dat','rb')
    key2=fs.read()
    fs.close()
    return (key1,key2)

def GetKey2(gamename):
    fs=open(gamename+'key.dat','rb')
    key=fs.read()
    fs.close()
    return key

#keys=GetKey(GameName)
key1 = GetKey2(GameName)
for f in os.listdir(path1):
    if not f.endswith('.rld'): continue
    if f=='def.rld':
        key2 = [0x16, 0xa9, 0x85, 0xae]
    else:
        key2 = [0x2a, 0x5b, 0x3c, 0x86]
    fs=open(path1+os.sep+f,'rb')
    stm=bytearray(fs.read())
    DecRld(stm, key1, key2)
    fs=open(path2+os.sep+f,'wb')
    fs.write(stm)
    fs.close()
