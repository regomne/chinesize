#encoding=gbk
import strfile
import zlib
import os
import pdb

dec_key=[0x4D, 0x61, 0x69, 0x64, 0x4C, 0x6F, 0x76, 0x65]#'MaidLove'

def extract(stm):
    dec=[]
    for i in range(len(stm)):
        dec.append(chr(ord(stm[i])^dec_key[i&7]))
    dec=''.join(dec)
    return zlib.decompress(dec[4:])

fs=open('ardscript.ard','rb')
stm=strfile.MyStr(fs.read())
fs.close()

if 'ArdScript' not in os.listdir('.'):
    os.mkdir('ArdScript')

stm.seek(4)
if stm.read(4)!='ARD0':
    print('格式错误')
count=stm.readu32()
stm.seek(0x100)
for i in range(count):
    off=stm.readu32()
    size=stm.readu32()
    resv=stm.readu32()
    name=stm.read(0x21).rstrip('\0')+'.txt'
    fs=file('ArdScript\\'+name,'wb')
    fs.write(extract(stm[off:off+size]).decode('932').encode('utf-16'))
    fs.close()
