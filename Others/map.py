import os
from struct import pack
from pdb import set_trace as int3

OriMap=['【','】','「','」','（','）','　']

def yieldSjisPos():
    high=0x88
    low=0x9f
    while True:
        yield (high<<8)|low
        low=low+1
        if low==0x7f:
            low=0x80
        if low==0x9d or low==0x9e:
            low=0x9f
        elif low>=0xfc:
            low=0x40
            high=high+1
            if high==0xa0:
                high=0xe0
            elif high>=0xfe:
                return

def getCode(ch):
    if len(ch)==1:
        return ch[0]
    elif len(ch)==2:
        return (ch[0]<<8)|ch[1]
    else:
        raise Exception("err")

def makeSjisMapTable(chars,orimap):
    maptbl=[0]*65536
    rev=[0]*65536
    for ch in orimap:
        maptbl[getCode(ch.encode('932'))]=ord(ch)
        rev[ord(ch)]=getCode(ch.encode('932'))
    i=0
    #int3()
    for sjch in yieldSjisPos():
        if maptbl[sjch]!=0:
            continue
        while rev[ord(chars[i])]!=0:
            i+=1
            if i>=len(chars):
                break
        maptbl[sjch]=ord(chars[i])
        rev[ord(chars[i])]=sjch
        i+=1
        if i>=len(chars):
            break
    return (maptbl,rev)


def serializeMapTbl(maptbl):
    buff=bytearray()
    for i in range(len(maptbl)):
        buff+=pack('H',maptbl[i])
    return buff

def mapChars(txt,rev):
    newt=bytearray()
    #int3()
    for ch in txt:
        ch=ord(ch)
        if rev[ch]!=0:
            newt+=pack('>H',rev[ch])
        else:
            if ch<0x100:
                newt+=bytes([ch])
            else:
                raise Exception("err2")
    return newt

path1='txt'
path2='ntxt'
def getChars(path1):
    chars=[]
    for f in os.listdir(path1):
        txt=open(os.path.join(path1,f),'rb').read().decode('u16')
        for c in txt:
            if ord(c)>=0x100 and (c not in chars):
                chars.append(c)
    return chars

def repFiles(path1,path2):
    chars=getChars(path1)
    maptbl,rev=makeSjisMapTable(chars,OriMap)
    fs=open('map.tbl','wb')
    fs.write(serializeMapTbl(maptbl))
    fs.close()
    for f in os.listdir(path1):
        txt=open(os.path.join(path1,f),'rb').read().decode('u16')
        newt=mapChars(txt,rev)
        fs=open(os.path.join(path2,f),'wb')
        fs.write(newt.decode('932').encode('u16'))
        fs.close()

repFiles(path1,path2)
