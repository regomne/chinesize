#encoding=utf-8
#py3.2
import os
from pdb import set_trace as int3

path1=r'D:\Program Files\DC3RX\Advdata\txt'
path2=r'D:\Program Files\DC3RX\Advdata\txt1'

puncs=['，','。','？','…','！','」','』','”']

def AddEnterForLine(line,maxchar,preschar):
    global puncs
    i=0
    newl=[]
    if (line[0]=='「') or (line[0]=='『'):
        quote=True
    else:
        quote=False
        
    ignore_next=False
    while i<len(line):
        if quote:
            if i==0:
                count=0
            else:
                count=1
                newl.append('　')
                if line[i]=='　':
                    i+=1
        else:
            count=0
        while i<len(line):
            c=line[i]
            if (c=='\\') and line[i+1]=='n':
                if not ignore_next:
                    newl.append('\\')
                    newl.append('n')
                    i+=2
                    break
                else:
                    if quote and (line[i+2]=='　'):
                        i+=3
                    else:
                        i+=2
            newl.append(line[i])
            count+=1
            i+=1
            if count>=maxchar:
                #int3()
                ignore_next=False
                pos=0
                for n in range(preschar):
                    if i+n>=len(line):
                        break
                    cc=line[i+n]
                    if cc in puncs:
                        newl.append(cc)
                        pos+=1
                    elif (cc=='\\') and (line[i+n+1]=='n'):
                        ignore_next=True
                        break
                    else:
                        break
                i+=pos
                if i!=len(line):
                    newl+=['\\','n']
                break
    return ''.join(newl)

def AddEnterForTxt(stm):
    lines=stm.decode('u16').split('\r\n')
    for i in range(len(lines)):
        l=lines[i]
        if len(l)!=0:
            l=AddEnterForLine(l,26,3)
            lines[i]=l
    return '\r\n'.join(lines).encode('u16')

for f in os.listdir(path1):
    if not f.endswith('.txt'):
        continue
    fs=open(os.path.join(path1,f),'rb')
    stm=fs.read()
    fs=open(os.path.join(path2,f),'wb')
    fs.write(AddEnterForTxt(stm))
    fs.close()
