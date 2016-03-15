#encoding=utf-8
import os
import bytefile
from bntParser import BntParser
from pdb import set_trace as int3


def ImpSingleTxt(name1,name2,name3):
    fs=open(name1,'rb')
    parser=BntParser(bytefile.ByteIO(fs.read()))
    fs.close()

    if not os.path.exists(name2):
        return

    fs=open(name2,'rb')
    lines=fs.read().decode('U16').split('\r\n')
    if lines==['']:
        return

    nl=parser.ExtTxt()[0:]

    j=0
    for i in range(len(nl)):
        l=nl[i].strip('\t')
        half=True
        for c in l:
            if ord(c)>0x80:
                half=False
                break
        if not half:
            nl[i]=lines[j].replace('・','·')
            j+=1
        
    stm=parser.ImpTxt(nl)

    rp=os.path.split(name3)[0]
    if rp!='':
        if not os.path.exists(rp):
            os.makedirs(rp)
    fs=open(name3,'wb')
    fs.write(stm[0:])
    fs.close()

#ImpSingleTxt('mission.bnt','mission.txt','mission1.bnt')

path1='rpack01'
path2='txt'
path3='rpack03'
pos1=len(path1)+1
pos2=len(path2)+1
pos3=len(path3)+1
for root,dirs,files in os.walk(path1):
    for f in files:
        if not f.endswith('.bnt'):
            continue
        rlname=os.path.join(root,f)[pos1:]
        ImpSingleTxt(os.path.join(path1,rlname),
                     os.path.join(path2,rlname.replace('.bnt','.txt')),
                     os.path.join(path3,rlname),)
