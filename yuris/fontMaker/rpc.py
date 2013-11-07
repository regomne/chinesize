#encoding=utf-8
import os
import random
import re
import struct
import pickle

txtpath=r'D:\chinesize\yuris\nt'
cvtpath=r'D:\chinesize\yuris\cvted'

def ReplaceText(stm,EscapeTbl):
    news=[]
    lines=stm.split('\r\n')
    #pat=re.compile(r'^(【[^】]+】)(.*)')
    for l in lines:
        nl=[]
        for i in range(len(l)):
            c=l[i]
            cs=EscapeTbl[ord(c)]
            if cs==0:
                nl.append(c)
            else:
                if (i<3) or (ord(c)<0x100):
                    nl.append(chr(cs[0]).encode('utf-16be').decode('936').strip('\0'))
                else:
                    nl.append(chr(cs[random.randint(0,len(cs)-1)]).encode('utf-16be').decode('936').strip('\0'))
        news.append(''.join(nl))
    return '\r\n'.join(news)

def ReplaceYurisNames(stm,EscapeTbl):
    lines=stm.split('\r\n')
    i=0
    while i<len(lines):
        l=lines[i]
        if len(l)==0:
            i+=1
            continue
        nl=[]
        for c in l:
            cs=EscapeTbl[ord(c)]
            if cs==0:
                nl.append(c)
            else:
                nl.append(chr(cs[0]).encode('utf-16be').decode('936').strip('\0'))
        lines[i]=''.join(nl)

        if i>=3:
            lines[i+1]=''.join(nl)
            #lines[i+1]=lines[i+1].encode('932').decode('936')
            #lines[i+2]=lines[i+2].encode('932').decode('936')
            i+=3
        else:
            i+=1
    return '\r\n'.join(lines)

et=pickle.load(open('et.tbl','rb'))

for f in os.listdir(txtpath):
    if f.lower().endswith('.txt') and f!='yst00069.txt':
        stm=open(os.path.join(txtpath,f),'rb').read().decode('u16')
        print('converting '+f)
        ns=ReplaceText(stm,et)
        open(os.path.join(cvtpath,f),'wb').write(ns.encode('u16'))


stm=open(os.path.join(txtpath,'yst00069.txt'),'rb').read().decode('u16')
ns=ReplaceYurisNames(stm,et)
open(os.path.join(cvtpath,'yst00069.txt'),'wb').write(ns.encode('u16'))
        
