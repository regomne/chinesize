#encoding=utf-8
from pdb import set_trace as int3
import os

path1='S\\'
path2='NewST\\'
path3='NewS\\'

def readarg(stc):
    arg=[]
    cur=stc.find('(')+1
    while cur<stc.rfind(')'):
        p=stc.find(',',cur)
        if p==-1:
            p=stc.rfind(')')
        elif '"' in stc[cur:p]:
            cur=stc.find('"',cur)
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
    stf=open(path2+f,'rb')
    txt=stf.read().decode('U16').encode('gbk','replace').split('\r\n')
    stf.close()
    print f

    t_li=0
    i=0
    while 1:
        stc=lines[i].strip()
        if stc.startswith('CreateBalloon'):
            j=i
            tabcount=lines[i].count('\t')
            while stc[-1]!=';':
                j+=1
                stc+=lines[j].strip()
            del lines[i:j+1]
            
            arg=readarg(stc)
            fun_name=stc[0:stc.find('(')]
            arg[8]='"'+txt[t_li]+'"'
            t_li+=1
            lines.insert(i,'\t'*tabcount+fun_name+'('+', '.join(arg)+');')
        i+=1
        if i>=len(lines): break

    fs=open(path3+f,'wb')
    fs.write('\r\n'.join(lines))
    fs.close()

print '\nSuccess!'
input()
