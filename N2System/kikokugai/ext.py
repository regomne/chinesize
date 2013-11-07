#py3.2

import os

path1='0nss'
path2='1nss'
path3='rubys'

def exp(lines):
    newl=[]
    i=0
    while i<len(lines):
        if len(lines[i].lstrip())<=1:
            i+=1
            continue
        elif lines[i].lstrip()[0]>=0x80:
            newl.append(lines[i])
            if len(lines[i+1])>0 and lines[i+1][0]>0x80:
                newl[-1]+=lines[i+1]
                i+=1
        i+=1
    newl.append(b'')
    return newl

def exp2(lines):
    newl=[]
    i=0
    while i<len(lines):
        if len(lines[i].lstrip())<=1:
            i+=1
            continue
        elif lines[i].lstrip()[0:2]==b'<R':
            newl.append(lines[i])
##            if len(lines[i+1])>0 and lines[i+1][0]>0x80:
##                newl[-1]+=lines[i+1]
##                i+=1
        i+=1
    newl.append(b'')
    return newl

for f in os.listdir(path1):
    fs=open(path1+os.sep+f,'rb')
    lines=fs.read().split(b'\r\n')
    fs.close()
    newl=exp(lines)
    if len(newl)>1:
        fs=open(path2+os.sep+f.replace('.nss','.txt'),'wb')
        fs.write(b'\r\n'.join(newl))#.decode('932').encode('U16'))
        fs.close()
    
