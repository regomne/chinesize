#encoding=utf-8
#py3.2

import os

path1='0nss'
path2='nss2'
path3='newnss'

def rpcSpace(line):
    newl=bytearray(line)
    i=0
    while i < len(newl):
        ch=newl[i]
        if ch==0xa1 and newl[i+1]==0xa1:
            newl[i]=0x81
            newl[i+1]=0x40
            i+=2
        elif ch>0x80:
            i+=2
        else:
            i+=1
    return newl
        

def pack(lines,tlines):
    i=0
    j=0
    newl=[]
    while i<len(lines):
        l=lines[i].lstrip()
        if len(l)==0:
            newl.append(lines[i])
            i+=1
            continue
        elif l[0]>=0x80:
            newl.append(rpcSpace(tlines[j]))
            j+=1
            if len(lines[i+1])>0 and lines[i+1][0]>0x80:
                i+=1
        else:
            newl.append(lines[i])
        i+=1

    newl.append(b'')
    return newl

dirs2=os.listdir(path2)
for f in os.listdir(path1):
    fs=open(path1+os.sep+f,'rb')
    lines=fs.read().split(b'\r\n')
    fs.close()
    
    newname=f.replace('.nss','完成.txt')
    if newname not in dirs2: continue
    print(f)
    fs=open(path2+os.sep+newname,'rb')
    tlines=fs.read().decode('U16').encode('936','replace').split(b'\r\n')
    fs.close()
    
    newl=pack(lines,tlines)
    fs=open(path3+os.sep+f,'wb')
    fs.write(b'\r\n'.join(newl))
    fs.close()
