#encoding=utf-8
import os
import sys
from pdb import set_trace as int3

path=sys.argv[1] #译文目录
path2=sys.argv[2] #原文目录


chs=[]

codes=['♪', '・', '≪','≫',' ']
codes2=['☆','·','《','》','　']

has_error = False

for f in os.listdir(path):
    fs=open(os.path.join(path,f),'rb')
    stm=fs.read().decode('u16')
    fs.close()
    fs=open(os.path.join(path2,f),'rb')
    ls1=fs.read().decode('u16').split('\r\n')
    cnt1=len(ls1)
    if ls1[-1]=='':
        cnt1-=1
    fs.close()
    try:
        stm.encode('936')
    except UnicodeError as e:
        c=stm[e.start:e.end]
        if c not in chs:
            chs.append(c)
    ls2=stm.split('\r\n')
    cnt2=len(ls2)
    if ls2[-1]=='':
        cnt2-=1
    if cnt1!=cnt2:
        print(f, '行数不一致')
        has_error = True
        continue
    
    ls=stm.split('\r\n')
    #int3()
    for i in range(len(ls)):
        for c in ls[i]:
            c=ord(c)
            if (c<0x100) and (c not in b'$f\\n@lm\xb7 \xd7'):
                print(f,i+1,'行有半角字符')
                has_error = True
                break
    for i in range(len(codes)):
        stm=stm.replace(codes[i],codes2[i])

    fs=open(os.path.join(path,f),'wb')
    fs.write(stm.encode('u16'))
    fs.close()

#print('已经自动转换过的字符:', chs)

if has_error:
    sys.exit(1)
sys.exit(0)
