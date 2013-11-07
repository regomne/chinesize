#py3.2

import os

path1='K:\\Program Files\\kikokugai\\0nss\\'
path2='K:\\Program Files\\kikokugai\\nss\\'

def rpcSpace(l):
    i=0
    nl=b''
    while i<len(l):
        if l[i]<0x80:
            nl+=l[i:i+1]
            i+=1
        elif l[i:i+2]==b'\xa1\xa1':
            nl+=b'\x81\x40'
            i+=2
        else:
            nl+=l[i:i+2]
            i+=2
    return nl

for f in os.listdir(path1):
    if not f.endswith('.nss'): continue
    fs1=open(path1+f,'rb')
    lines=fs1.read().split(b'\r\n')
    fs1.close()
    newl=[]
    for l in lines:
        if len(l)<=2:
            newl.append(l)
        else:
            newl.append(rpcSpace(l))
    fs2=open(path2+f,'wb')
    fs2.write(b'\r\n'.join(newl))
    fs2.close()
