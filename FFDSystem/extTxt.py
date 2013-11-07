#encoding=utf-8
#py2.7
import os
from pdb import set_trace as int3

path1='ST\\'
path2='SPT\\'
path3='STF\\'

for f in os.listdir(path1):
    fs=open(path1+f,'rb')
    lines=fs.read().decode('utf-16').replace('<BR>','\\n').split('\r\n')
    fs.close()
    ptext=[]
    tags=[]
    print f
    for li in lines:
        text=''
        cur=0
        while cur<len(li):
            if li[cur]=='<':
                p=cur
                while li[p]=='<':
                    p=li.find('>',p)+1
                    if p==-1: int3()
                    if p==len(li): break
                tags.append(li[cur:p])
                text+='@'
                cur=p
            else:
                p=li.find('<',cur)
                if p==-1:
                    p=len(li)
                text+=li[cur:p]
                cur=p
        ptext.append(text)

    textf=open(path2+f,'wb')
    textf.write('\r\n'.join(ptext).encode('utf-16'))
    textf.close()

    tagf=open(path3+f,'wb')
    tagf.write('\r\n'.join(tags).encode('utf-16'))
    tagf.close()

print 'Success!'
input()
