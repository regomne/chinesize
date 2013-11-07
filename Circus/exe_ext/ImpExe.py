#encoding=utf-8
import bytefile
import sys

def ImpExe(exename,idxname,txtname,newname):
    fs=open(idxname,'rb')
    idxstm=bytefile.ByteIO(fs.read())
    idx=[]
    while idxstm.tell()<len(idxstm):
        idx.append(idxstm.readu32())

    fs=open(txtname,'rb')
    lines=fs.read().split(b'\r\n')

    if len(lines)!=len(idx):
        print('Lines not match')
        sys.exit()

    fs=open(exename,'rb')
    nfs=open(newname,'wb')
    nfs.write(fs.read())
    nfs.close()
    fs.close()

    fs=open(newname,'rb+')
    for i in range(len(idx)):
        fs.seek(idx[i])
        l=lines[i]#.replace('・'.encode('936'),'·'.encode('936'))
        fs.write(l+b'\0')

    fs.close()

ImpExe('sakura_chs2.exe',\
       'exe.idx',\
       'exen.txt',\
       'cn.exe')
