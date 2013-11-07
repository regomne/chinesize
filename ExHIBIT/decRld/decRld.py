#encoding=utf-8
#py3.2

import os
import bytefile

path1=r'F:\Program Files\pochi\rld'
path2=r'F:\Program Files\pochi\rld2'
GameName='ぽちとご主人様'

def DecRld(stm,key):
    if len(stm)-0x10>0x3ff0*4:
        declen=0x3ff0*4
    else:
        declen=(len(stm)-0x10)&0xfffffffc
    for i in range(declen):
        stm[i+0x10]^=key[i&0x3ff]


def GetKey(gamename):
    fs=open(gamename+'key1.dat','rb')
    key1=fs.read()
    fs.close()
    fs=open(gamename+'key2.dat','rb')
    key2=fs.read()
    fs.close()
    return (key1,key2)

keys=GetKey(GameName)
for f in os.listdir(path1):
    if not f.endswith('.rld'): continue
    if f=='def.rld':
        curkey=keys[0]
    else:
        curkey=keys[1]
    fs=open(path1+os.sep+f,'rb')
    stm=bytearray(fs.read())
    DecRld(stm,curkey)
    fs=open(path2+os.sep+f,'wb')
    fs.write(stm)
    fs.close()
