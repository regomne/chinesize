#encoding=utf-8
from pdb import set_trace as int3
import os

path1='S\\'
path2='ST\\'

def readarg(stc):
    arg=[]
    cur=stc.find('(')+1
    while cur<stc.rfind(')'):
        p=stc.find(',',cur)
        if p==-1:
            p=stc.rfind(')')
        elif '"' in stc[cur:p]:
            cur=stc.find('"',cur)
##            p=stc.find('"',cur+1)
##            while stc[p-1]=='\\' and ord(stc[p-2])<0x80:
##                p=stc.find('"',p+1)
            p=cur+1
            while p<len(stc):
                ch=ord(stc[p])
                if ch==0x5c and stc[p+1]=='"':
                    p+=2
                elif ch==0x22:
                    break
                elif (ch>=0x81 and ch<=0x9f) or (ch>=0xe0 and ch<=0xfc):
                    p+=2
                else:
                    p+=1
            p=stc.find(',',p)
            if p==-1:
                p=stc.rfind(')')
        arg.append(stc[cur:p].strip())
        cur=p+1
    return arg

for f in os.listdir(path1):
    fs=open(path1+f,'rb')
    lines=fs.read().split('\r\n')
    fs.close()
    newtxt=[]
    print f
    for i in range(len(lines)):
        stc=lines[i].strip()
        if stc.startswith('CreateBalloon'):
            while stc[-1]!=';':
                i+=1
                stc+=lines[i].strip()
            arg=readarg(stc)
            newtxt.append(arg[8].strip('"'))

    fs=open(path2+f,'wb')
    fs.write('\r\n'.join(newtxt).decode('932').encode('utf-16'))
    fs.close()

print 'Success'
input()
