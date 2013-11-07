#encoding=utf-8
#py2.7
import os
from pdb import set_trace as int3

path1='ST\\'
path2='SPT\\'
path3='STF\\'
path4='NewST\\'

def repAt(text,tags):
    newtxt=[]
    cur=0
    i=0
    while cur<len(text):
        p=text.find('@',cur)
        newtxt.append(text[cur:p])
        if p==-1:
            break
        newtxt.append(tags[i])
        i+=1
        cur=p+1
    return ''.join(newtxt)

for f in os.listdir(path1):
    fs=open(path1+f,'rb')
    lines=fs.read().decode('utf-16').replace('<BR>','\\n').split('\r\n')
    fs.close()
    textf=open(path2+f,'rb')
    ptext=textf.read().decode('utf-16')
    textf.close()
    tagf=open(path3+f,'rb')
    tags=tagf.read().decode('utf-16').split('\r\n')
    tagf.close()
    print f
    
    if len(tags)!=ptext.count('@') and (len(tags)>1 or len(tags[0])!=0):
        print '标记数目不匹配！'
        int3()
    newtxt=repAt(ptext,tags)
    newf=open(path4+f,'wb')
    newf.write(newtxt.replace('\\n','<BR>').encode('utf-16'))
    newf.close()

print '\nSuccess!'
input()
