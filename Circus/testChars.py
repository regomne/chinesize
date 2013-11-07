#encoding=utf-8
import os
from pdb import set_trace as int3

path=r'D:\Program Files\けい☆てん\AdvData\mest1'
path2=r'D:\Program Files\けい☆てん\AdvData\mest0'


chs=[]

codes=['♪', '・', '≪','≫',' ']
codes2=['☆','·','《','》','　']

for f in os.listdir(path):
    fs=open(os.path.join(path,f),'rb')
    stm=fs.read().decode('u16')
    fs.close()
    fs=open(os.path.join(path2,f),'rb')
    cnt0=len(fs.read().decode('u16').split('\r\n'))
    fs.close()
    try:
        stm.encode('936')
    except UnicodeError as e:
        c=stm[e.start:e.end]
        if c not in chs:
            chs.append(c)
    if len(stm.split('\r\n'))!=cnt0:
        print(f, 'line not fit')
        continue
    
    ls=stm.split('\r\n')
    #int3()
    for i in range(len(ls)):
        for c in ls[i]:
            c=ord(c)
            if (c<0x100) and (c not in b'\\n@lm\xb7 \xd7'):
                print(f,i+1)
                break
    for i in range(len(codes)):
        stm=stm.replace(codes[i],codes2[i])

    fs=open(os.path.join(path,f),'wb')
    fs.write(stm.encode('u16'))
    fs.close()

print(chs)
    

