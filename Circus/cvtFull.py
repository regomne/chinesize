import os

path1=r'D:\Program Files\けい☆てん\AdvData\mest'
path2=r'D:\Program Files\けい☆てん\AdvData\mest1'

def cvtFull(line):
    nl=[]
    i=0
    while i<len(line):
        c=ord(line[i])
        if c==ord('\\'):
            nl+=line[i:i+2]
            i+=2
            continue
        elif c==0x20:
            nl.append('　')
        elif c<0x80:
            nl.append(chr(c+0xfee0))
        else:
            nl.append(line[i])
        i+=1
    return ''.join(nl)

def cvtTxt(stm):
    lines=stm.decode('u16').split('\r\n')
    for i in range(len(lines)):
        l=lines[i]
        if len(l)!=0:
            l=cvtFull(l)
            lines[i]=l
    return '\r\n'.join(lines).encode('u16')

for f in os.listdir(path1):
    if not f.endswith('.txt'):
        continue
    fs=open(os.path.join(path1,f),'rb')
    stm=fs.read()
    fs=open(os.path.join(path2,f),'wb')
    fs.write(cvtTxt(stm))
    fs.close()
