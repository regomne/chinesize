import struct
import bytefile
import sys

def Encode(bstr):
    nstr=[]
    for i in range(len(bstr)):
        nstr.append((bstr[i]-0x20)&0xff)
    return bytes(nstr)

def ImpSingle(mesname,txtname,idxname,newname):
    fs=open(mesname,'rb')
    stm=bytearray(fs.read())
    fs=open(idxname,'rb')
    idxstm=bytefile.ByteIO(fs.read())
    idx=[]
    while idxstm.tell()<len(idxstm):
        isdec=idxstm.readu32()
        off=idxstm.readu32()
        idx.append((isdec,off))

    fs=open(txtname,'rb')
    lines=fs.read().split(b'\r\n')
    if len(lines)!=len(idx):
        print('Lines not match')
        sys.exit()

    newstm=bytearray()
    cur=0
    i=0
    for isdec,off in idx:
        newstm+=stm[cur:off]
        p=stm.find(b'\0',off)
        if p==-1:
            print('Idx error')
            sys.exit()
        cur=p+1

        s=lines[i].decode('936').replace('\\n','\n').encode('936')
        i+=1
        if isdec==1:
            s=Encode(s)

        newstm+=s+b'\0'
    newstm+=stm[cur:]

    fs=open(newname,'wb')
    fs.write(newstm)
    fs.close()

ImpSingle('mes\\option.mes','txt\\option.txt','txt\\option.idx','option.mes')
