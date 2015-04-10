#encoding=utf-8
import os
import shutil
import zlib
from pdb import set_trace as int3

path1='patch'
path2='patch20'

path1s=[#"arc\\image",
    #"sys_patch","sys_patch2",
    "birthday_vol1","birthday_vol2","birthday_vol3","birthday_vol4","lc2_p5",
    "lc2_p6","lc2_p7","lc2_p8","lc2_p9","lc2_p10","lc2_p11","lc2_p12","lc2_p1",
    "lc2_p2","lc2_p3","lc2_p4",
    ]
path2s='myp2'

def isNeedCvt(fname):
    ext=fname.split('.')[-1].lower()
    fs=open(fname,'rb')
    if fs.read(2)==b'\xff\xfe':
        fs.close()
        return False
    if ext in exts1:
        return True
    fs.seek(0)
    stm=fs.read()
    fs.close()
    if len(stm.decode('932'))!=len(stm):
        return True
    return False

def Decrypt(stm):
    if stm[0:5]==b'\xfe\xfe\x01\xff\xfe':
        ns=bytearray(stm[5:])
        for i in range(len(ns)):
            c=ns[i]
            if c:
                ns[i]=(((c&0x55)<<1) | ((c&0xaa)>>1))&0xff
        return ns.decode('utf-16le')
    elif stm[0:5]==b'\xfe\xfe\x02\xff\xfe':
        return zlib.decompress(stm[0x15:]).decode('u16')
        
    
def moveto(src,dest):
    fs=open(src,'rb')
    stm=fs.read()
    fs.close()
    try:
        if stm[0:2]=='\xfe\xfe': ##这里控制加密(需要加b'')ks else下面也需要一起改
            stm=Decrypt(stm)
        else:
            ##raise Exception("kinshi")
            stm=stm.decode('932')
    except Exception as e:
        print(src,e)
        return
    fs=open(dest,'wb')
    fs.write(stm.encode('U16'))
    fs.close()
    
exts1=['ks','txt','csv','tjs','ini','func']
exts2=['anm','asd','pos','stage']

ExtData=True
if not ExtData:
    for p1 in path1s:
        for f in os.listdir(p1):
            ext=f.split('.')[-1].lower()
            if (ext in exts1) or (ext in exts2):
                if isNeedCvt(os.path.join(p1,f)):
                    moveto(os.path.join(p1,f),os.path.join(path2s,f))
else:

    for root,dirs,files in os.walk(path1):
        for f in files:
            ext=f.split('.')[-1].lower()
            if (ext in exts1) or (ext in exts2):
                if isNeedCvt(os.path.join(root,f)):
                    moveto(os.path.join(root,f),os.path.join(path2,f))

