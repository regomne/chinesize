import os
from struct import unpack,pack

def Pack(path,newname):
    if path.endswith('\\'):
        pos=len(path)
    else:
        pos=len(path)+1

    idxlen=4
    for root,dirs,files in os.walk(path):
        for f in files:
            name=os.path.join(root[pos:],f).encode('932')
            idxlen+=len(name)+1+0x18
            
    off=idxlen+0x24
    
    newf=open(newname,'wb')
    entries=[]
    for root,dirs,files in os.walk(path):
        for f in files:
            ent=[0,0,0,0,0,0,0]
            fs=open(os.path.join(root,f),'rb')
            name=os.path.join(root[pos:],f).encode('932')
            ent[0]=name+b'\0'
            fs.seek(0,2)
            ent[1]=off
            ent[2]=fs.tell()
            ent[3]=fs.tell()
            ent[4]=fs.tell()
            
            newf.seek(ent[1])
            fs.seek(0)
            newf.write(fs.read())
            fs.close()

            off+=ent[4]
            entries.append(ent)

    idxstm=bytearray()
    idxstm+=pack('I',len(entries))
    for ent in entries:
        idxstm+=ent[0]
        idxstm+=pack('<QIIIHH',ent[1],ent[2],ent[3],ent[4],ent[5],ent[6])
        
    
    newf.seek(0)
    newf.write(b'mashiroiro\0'.ljust(0x20))
    newf.write(pack('I',idxlen))
    if len(idxstm)!=idxlen:
        a+3
    newf.write(idxstm)
    newf.close()

Pack(r'E:\Program Files\MASHIRO\sys',
     r'E:\Program Files\MASHIRO\sys1.dat')
