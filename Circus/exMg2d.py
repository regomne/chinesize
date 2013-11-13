#encoding=utf-8
import struct

def DecTxt(s):
    ns=[]
    for c in s:
        ns.append(c^0xff)
    return bytes(ns)

def GetTxts(stm,cnt):
    txt=[]
    for i in range(cnt):
        s=stm.read(0x344)
        p=s.find(b'\0',0)
        if p==-1:
            raise "sdf"
        txt.append(DecTxt(s[0:p]).decode('932'))

        start=0x104
        for j in range(9):
            p=s.find(b'\0',start)
            if p==-1:
                raise "sdf1"
            if p!=start:
                txt.append(DecTxt(s[start:p]).decode('932'))
            start+=0x40
    return txt

def PackTxts(stm,cnt,txt):
    ns=[]
    lc=0
    hasErr=False
    for i in range(cnt):
        s=bytearray(stm.read(0x344))
        l=DecTxt(txt[lc].encode('936'))+b'\0'
        lc+=1
        if len(l)>0x100:
            print("too long %d"%lc)
            hasErr=True
        else:
            s[0:len(l)]=l
        start=0x104
        for j in range(9):
            p=s.find(b'\0',start)
            if p==-1:
                raise "sdf1"
            if p!=start:
                l=DecTxt(txt[lc].encode('936'))+b'\0'
                lc+=1
                if len(l)>0x40:
                    print("too long %d"%lc)
                    hasErr=True
                else:
                    s[start:start+len(l)]=l
            start+=0x40
        ns.append(s)
    return b''.join(ns)

isPack=True

if not isPack:
    fs=open(r'D:\Program Files\けい☆てん\AdvData\EX\quiz_list.dat','rb')
    fs.seek(4)
    cnt,=struct.unpack('I',fs.read(4))
    txt=GetTxts(fs,cnt)
    fs=open('a.txt','wb')
    fs.write('\r\n'.join(txt).encode('u16'))
    fs.close()
else:
    fs=open(r'D:\Program Files\けい☆てん\AdvData\EX\quiz_list.dat','rb')
    fs.seek(4)
    cnt,=struct.unpack('I',fs.read(4))
    fs2=open(r'D:\Program Files\けい☆てん\AdvData\EX\quiz.txt','rb')
    txt=fs2.read().decode('u16').replace('・','·').split('\r\n')
    ns=PackTxts(fs,cnt,txt)
    fs2=open(r'D:\Program Files\けい☆てん\AdvData\EX\quiz_list1.dat','wb')
    fs.seek(0)
    fs2.write(fs.read(8))
    fs2.write(ns)
    fs2.close()
