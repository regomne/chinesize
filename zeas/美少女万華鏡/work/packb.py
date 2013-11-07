#encoding=utf-8
import os
import struct

def PackAFile(fname):
    fs=open(fname,'rb')
    stm=fs.read()
    fs.close()
    entry=bytearray()
    entry+=b'abimgdat10'.ljust(16,b'\0')
    p=fname.rfind('\\')
    if p==-1:
        pn=fname.encode('932')
    else:
        pn=fname[p+1:].encode('932')
    entry+=struct.pack('H',len(pn))
    entry+=pn
    entry+=b'\x03'
    entry+=struct.pack('I',len(stm))
    entry+=stm
    return entry

def GetOriHdr(fname):
    fs=open(fname,'rb')
    stm=fs.read(0x24)
    hdrlen,=struct.unpack('32xI',stm)
    hdrs=fs.read(hdrlen)
    return stm+hdrs

def WriteB(path1,oriname,newname):
    cnt=0
    nnames=[]
    for f in os.listdir(path1):
        nnames.append(f)
        cnt+=1
    hdr=GetOriHdr(oriname)
    fs=open(oriname,'rb')
    fs.seek(len(hdr))
    imgsect=fs.read(17)
    magic,cnnt=struct.unpack('<16sB',imgsect)
    if magic.strip(b'\0')!=b'abimage10':
        print(magic)
        asdgawer
    if cnnt!=cnt:
        print('not match,count')
        afasef

    imgstm=bytearray(imgsect)
    for i in range(cnt):
        magic,namelen=struct.unpack('16sH',fs.read(18))
        na=fs.read(namelen)
        if magic.strip(b'\0')==b'abimgdat13':
            hashlen,=struct.unpack('H',fs.read(2))
            hashc=fs.read(hashlen)
            na+=struct.pack('H',hashlen)+hashc
            na+=fs.read(13)
        elif magic.strip(b'\0')==b'abimgdat11':
            awerfgasdf
        else:
            na+=fs.read(1)
        imgstm+=magic+struct.pack('H',namelen)+na

        olen,=struct.unpack('I',fs.read(4))
        fs.seek(olen,1)

        nstm=open(os.path.join(path1,nnames[i]),'rb').read()
        imgstm+=struct.pack('I',len(nstm))+nstm
    nfs=open(newname,'wb')
    nfs.write(hdr)
    nfs.write(imgstm)
    nfs.write(fs.read())
    nfs.close()
    fs.close()

newfolders='bfiles'
oribf='orib'
newbf='newb'

for f in os.listdir(oribf):
    WriteB(os.path.join(newfolders,f),
           os.path.join(oribf,f),
           os.path.join(newbf,f))

##WriteB(r'D:\chinesize\zeas\美少女万華鏡\exp\button_yes.b',
##       r'D:\Program Files\美少女万華鏡\GameData\da7\library\dialog\button_yes.b',
##       r'nbutton_yes.b')
