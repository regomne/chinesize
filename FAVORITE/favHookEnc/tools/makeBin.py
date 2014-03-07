#encoding=utf-8
import os
from struct import unpack,pack
from Crypto.Cipher import AES
from Crypto import Random
from Crypto.Hash import MD5
import binascii
import math
from pdb import set_trace as int3

def RepChars(l):
    return l.replace('・','·')

def ZeroPad(text):
    return text.ljust((int((len(text)-1)/16)+1)*16,b'\0')

def ReadDict(path):
    texts=[]
    for f in os.listdir(path):
        if not f.endswith('.idx'):
            continue
        idxs=open(os.path.join(path,f),'rb').read()
        ls=open(os.path.join(path,f.replace('.idx','.txt')),'rb').read().\
            decode('u16').split('\r\n')
        if len(idxs)/4!=len(ls):
            print(f,' lines err')
            continue
        for i in range(len(ls)):
            try:
                l=RepChars(ls[i]).encode('936')
            except:
                print(f,' Line: ',i+1)
                continue
            texts.append((unpack('>I',idxs[i*4:i*4+4])[0],l))

    return sorted(texts)

def keyGen1(off):
    h=MD5.new()
    h.update(str(off).encode())
    return h.digest()

def keyGen2(off):
    h=MD5.new()
    h.update(str(off).encode())
    return h.hexdigest()[0:16].encode()

def keyGen3(off):
    c=[0]*16
    for i in range(16):
        c[i]=((off%97)*(i+1))&0xff
    return bytes(c)

def keyGen4(off):
    c=[0]*16
    for i in range(16):
        k=(off>>((i&3)*8))&0xff
        c[i]=(k*k*(i+7))&0xff
    return bytes(c)

def EncTexts(texts):
    keyAlgo=[keyGen1,keyGen2,keyGen3,keyGen4]
    randg=Random.new()
    st=0
    for i in range(len(texts)):
        #int3()
        off,text=texts[i]
        alidx=(off&0x30000)>>16
        key=keyAlgo[alidx](off)
        cipher=AES.new(key,AES.MODE_ECB)
        salt=randg.read(4)
        texts[i]=(off,cipher.encrypt(ZeroPad(salt+text)))
        if i/len(texts)*100>st:
            print(st,'%')
            st+=1

def PackTexts(texts,fs):
    fs.write(pack('<I',len(texts)))
    for off,text in texts:
        fs.write(pack('<IH',off,len(text)))
        fs.write(text)

texts=ReadDict('txt')
EncTexts(texts)
fs=open('cnpack','wb')
PackTexts(texts,fs)
append=open('crypto.bin','rb').read()
fs.write(append)
fs.close()
