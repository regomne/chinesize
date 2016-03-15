#encoding=utf-8
import os
import struct

def Dec(stm):
    for i in range(len(stm)):
        stm[i]^=0xa4
        
def Pack(path,fname):
    path=path.rstrip('\\')
    pos=len(path)+1

    ents=[]
    off=0
    idxlen=0
    for root,dirs,files in os.walk(path):
        for f in files:
            ent=[0,0,0,0,0]
            fpath=os.path.join(root,f)
            ent[0]=fpath[pos:].replace('\\','/')
            idxlen+=4+len(ent[0].encode('932'))+1+16
            ent[1]=off
            ent[3]=os.path.getsize(fpath)
            off+=ent[3]
            ents.append(ent)

    newf=open(fname,'wb')
    newf.seek(idxlen+0x40)
    for ent in ents:
        fs=open(os.path.join(path,ent[0]),'rb')
        #fs=open(ent[0],'rb')
        stm=bytearray(fs.read())
        #Dec(stm)
        newf.seek(ent[1]+idxlen+0x40)
        newf.write(stm)
        fs.close()

    idxstm=bytearray()
    for ent in ents:
        nm=ent[0].encode('932')
        idxstm+=struct.pack('I',len(nm))
        idxstm+=nm+b'\0'
        idxstm+=struct.pack('IIII',ent[1],ent[2],ent[3],ent[4])

    if len(idxstm)!=idxlen:
        a+=1

    hdr=bytearray()
    hdr+=struct.pack('<8sIIII',b'mgbpack\0',1,0x40,len(ents),idxlen)
    hdr=hdr.ljust(0x40,b'\0')
    Dec(hdr)
    newf.seek(0)
    newf.write(hdr)

    Dec(idxstm)
    newf.write(idxstm)
    newf.close()

Pack(r'E:\program files\妖々剣戟夢想\resource\rpack03',
     r'E:\program files\妖々剣戟夢想\resource\rpack03.dat')
