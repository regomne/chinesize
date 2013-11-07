import bytefile
import struct

def Decode(bstr):
    for i in range(len(bstr)):
        bstr[i]=(bstr[i]+0x20)&0xff
    return bstr

def WriteIdx(idx,idxname):
    idxstm=bytearray()
    for isdec,i in idx:
        idxstm+=struct.pack('II',isdec,i)

    fs=open(idxname,'wb')
    fs.write(idxstm)
    fs.close()

    
def ExpOptions(fname,txtname,idxname=''):

    fs=open(fname,'rb')
    stm=bytefile.ByteIO(fs.read())
    fs.close()

    i=0
    txt=[]
    idx=[]
    while i<len(stm)-3:
        if stm[i:i+3]==b'E@s':
            stm.seek(i+3)
            idx.append((0,stm.tell()))
            txt.append(stm.readstr().decode('932'))
            i=stm.tell()
            continue
        elif stm[i:i+3]==b'L S':
            stm.seek(i+3)
            idx.append((1,stm.tell()))
            txt.append(Decode(stm.readstr()).decode('932'))
            i=stm.tell()
            continue
        i+=1
    fs=open(txtname,'wb')
    fs.write('\r\n'.join(txt).encode('936'))
    fs.close()
    
    if idxname=='':
        idxname=txtname.replace('.txt','.idx')
        if idxname==txtname:
            idxname+='.idx'
    WriteIdx(idx,idxname)
    
def ExpMusic(fname,txtname,idxname=''):

    fs=open(fname,'rb')
    stm=bytefile.ByteIO(fs.read())
    fs.close()

    i=0
    txt=[]
    idx=[]
    while i<len(stm)-2:
        if stm[i]==0x4c:
            if (stm[i+1]>0x60) and (stm[i+1]<0xdf) and\
               (stm[i+2]>=0x20) and (stm[i+2]<0xdf):
                stm.seek(i+1)
                idx.append((1,stm.tell()))
                s=stm.readstr()
                Decode(s)
                txt.append(s.decode('932').replace('\n','\\n'))
                i=stm.tell()
                continue
        i+=1
    
    fs=open(txtname,'wb')
    fs.write('\r\n'.join(txt).encode('936','replace'))
    fs.close()
    
    if idxname=='':
        idxname=txtname.replace('.txt','.idx')
        if idxname==txtname:
            idxname+='.idx'
    WriteIdx(idx,idxname)

ExpOptions('mes\\option.mes','txt\\option.txt')
ExpMusic('mes\\music.mes','txt\\music.txt')
