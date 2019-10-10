#encoding=utf-8
#py3.2

import os


def DecRld(stm, key):
    if len(stm)-0x10 > 0x3ff0*4:
        declen = 0x3ff0*4
    else:
        declen = (len(stm)-0x10) & 0xfffffffc
    for i in range(declen):
        stm[i+0x10] ^= key[i & 0x3ff]


def GetKey(gamename, i1, i2):
    fs = open(gamename+'key1.dat', 'rb')
    key1 = bytearray(fs.read())
    fs.close()
    fs = open(gamename+'key2.dat', 'rb')
    key2 = bytearray(fs.read())
    fs.close()
    for i in range(len(key2)):
        key1[i & 0x3ff] ^= i1[i & 3]
        key2[i & 0x3ff] ^= i2[i & 3]
    return (key1, key2)


def GetKey2(gamename, i1, i2):
    fs = open(gamename+'key.dat', 'rb')
    key1 = bytearray(fs.read())
    fs.close()
    key2 = bytearray(key1)
    for i in range(len(key2)):
        key1[i & 0x3ff] ^= i1[i & 3]
        key2[i & 0x3ff] ^= i2[i & 3]
    return key1, key2


path1 = r'E:\BaiduDownload\SexOpenWorld\rld'
path2 = r'E:\BaiduDownload\SexOpenWorld\rld2'

key1, key2 = GetKey('sexopenworld',
                    [0x16, 0xa9, 0x85, 0xae],
                    [0x27, 0x97, 0x1b, 0x5a])
for f in os.listdir(path1):
    if not f.endswith('.rld'):
        continue
    fs = open(path1+os.sep+f, 'rb')
    stm = bytearray(fs.read())
    fs.close()
    if f == 'def.rld':
        DecRld(stm, key1)
    else:
        DecRld(stm, key2)
    fs = open(path2+os.sep+f, 'wb')
    fs.write(stm)
    fs.close()
