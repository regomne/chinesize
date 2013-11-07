##py3.2
import struct
import pdb
import strfile

RDATA_BASE=0x401200
DATA_BASE=0x401600

def extName(stm):
    newtxt=[]
    stm.seek(0x621c28-DATA_BASE+4)
    cur=stm.tell()
    for i in range(13):
        stm.seek(stm.readu32()-RDATA_BASE)
        newtxt.append(stm.readstr())
        cur+=12
        stm.seek(cur)
    return b'\r\n'.join(newtxt)

def extMail(stm):
    newtxt=[]
    stm.seek(0x643d68-DATA_BASE+0x34)
    cur=stm.tell()
    for i in range(0x27e):
        sub,mailto,text=struct.unpack('III',stm.read(12))
        if sub:
            stm.seek(sub-RDATA_BASE)
            newtxt.append(b'S '+stm.readstr().replace(b'\n',b'\\n').replace(b'\r',b'\\r'))
        if mailto:
            stm.seek(mailto-RDATA_BASE)
            newtxt.append(b'N '+stm.readstr().replace(b'\n',b'\\n').replace(b'\r',b'\\r'))
        if text:
            stm.seek(text-RDATA_BASE)
            newtxt.append(b'T '+stm.readstr().replace(b'\n',b'\\n').replace(b'\r',b'\\r'))
        cur+=0x40
        stm.seek(cur)
    return b'\r\n'.join(newtxt)

def extStr(stm,addr,count):
    newtxt=[]
    cur=addr-DATA_BASE
    for i in range(count):
        stm.seek(cur)
        stm.seek(stm.readu32()-RDATA_BASE)
        newtxt.append(stm.readstr().replace(b'\n',b'\\n').replace(b'\r',b'\\r'))
        cur+=4
    return b'\r\n'.join(newtxt)

def extBBS(stm):
    newtxt=[]
    start=0x61cb08-DATA_BASE
    for i in range(0x1b):
        cur=start+i*768
        while 1:
            stm.seek(cur)
            addr=stm.readu32()
            if addr==0:
                break
            stm.seek(addr-RDATA_BASE)
            newtxt.append(stm.readstr().replace(b'\n',b'\\n').replace(b'\r',b'\\r'))
            cur+=4
        newtxt.append(b'/'+b'*'*60+b'/')
    return b'\r\n'.join(newtxt)

fs=open('STEINSGATE.exe','rb')
stm=strfile.MyStr(fs.read())
fs.close()

names=extName(stm)
fs=open('name.txt','wb')
fs.write(names.decode('932').encode('utf-16'))
fs.close()

phone_info=extStr(stm,0x621d08,82)
fs=open('info.txt','wb')
fs.write(phone_info.decode('shift-jis').encode('utf-16'))
fs.close()

mail=extMail(stm)
fs=open('mail.txt','wb')
fs.write(mail.decode('shift-jis').encode('utf-16'))
fs.close()

bbs=extBBS(stm)
fs=open('bbs.txt','wb')
fs.write(bbs.decode('932').encode('utf-16'))
fs.close()
