#encoding=utf-8
import os
import random
import re
import struct
import pickle

txtpath=r'D:\chinesize\yuris\nt'
cvtpath=r'D:\chinesize\yuris\cvted'

def GetFileChars(stm,chars):
    for c in stm:
        if ord(c)<0x20:
            continue
        if c not in chars:
            chars.append(c)

def GetFilesChars(path):
    chars=[]
    for f in os.listdir(path):
        if f.lower().endswith('.txt'):
            fs=open(os.path.join(path,f),'rb')
            stm=fs.read().decode('u16')
            GetFileChars(stm,chars)
            fs.close()
    return chars

def isSjis(c):
    high=(c&0xff00)>>8
    low=c&0xff
    if not (((high>=0x81) and (high<=0x9f)) or ((high>=0xe0) and (high<=0xef))):
        return False
    if not ((low>=0x40) and (low<=0xfc) and (low!=0x7f)):
        return False
    return True

def RandomIt(chars):
    newc=[]
    mark=[0 for i in range(len(chars))]
    for i in range(len(chars)):
        j=random.randint(0,len(chars)-1)
        if mark[j]==1:
            k=1
            while True:
                if j+k<len(chars):
                    if mark[j+k]==0:
                        mark[j+k]=1
                        newc.append(chars[j+k])
                        break
                if j-k>=0:
                    if mark[j-k]==0:
                        mark[j-k]=1
                        newc.append(chars[j-k])
                        break
                k+=1
        else:
            newc.append(chars[j])
            mark[j]=1
    return newc

def ReadPreChars():
    chars=[]
    stm=open('name.txt','rb').read().decode('u16')
    GetFileChars(stm,chars)
    stm=open('ext.txt','rb').read().decode('u16')
    GetFileChars(stm,chars)

    return chars

def GenerateMapTable(chars,prechars):
    QueryTbl=[0 for i in range(65536)]
    QueryTbl2=QueryTbl[0:]
    EscapeTbl=QueryTbl[0:]
    mark=QueryTbl[0:]

    if(len(chars)>=0x1000):
        asdfa3e

    i=0x8201
    rptTbl=QueryTbl[0:]
    for c in chars:
        while not isSjis(i):
            i+=1
        EscapeTbl[ord(c)]=[i]
        QueryTbl[i]=random.randint(1,65000)
        while rptTbl[QueryTbl[i]]:
            QueryTbl[i]=random.randint(1,65000)
        rptTbl[QueryTbl[i]]=1
        QueryTbl2[ord(c)]=QueryTbl[i]
        i+=1

    for c in prechars:
        while not isSjis(i):
            i+=1
        if type(c)==tuple:
            ci=ord(c[0])
            if ci<0x100:
                EscapeTbl[ci]=[ci]
                QueryTbl[ci]=QueryTbl2[ord(c[1])]
            else:
                EscapeTbl[ord(c[0])].insert(0,i)
                QueryTbl[i]=QueryTbl2[ord(c[1])]
        else:
            ci=ord(c)
            if ci<0x100:
                EscapeTbl[ci]=[ci]
                QueryTbl2[ci]=ci
                QueryTbl[ci]=ci
            else:
                EscapeTbl[ord(c)].insert(0,i)
                QueryTbl[i]=QueryTbl2[ord(c)]
        i+=1
    
    while i<0xeffc:
        while not isSjis(i):
            i+=1
        c=chars[random.randint(0,len(chars)-1)]
        EscapeTbl[ord(c)].append(i)
        QueryTbl[i]=QueryTbl2[ord(c)]
        i+=1

    return (QueryTbl,QueryTbl2,EscapeTbl)

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
            lines[i+2]=lines[i+2].encode('932').decode('936')
            i+=3
        else:
            i+=1
    return '\r\n'.join(lines)

print('Collecting Files...')
chs=GetFilesChars(txtpath)
GetFileChars(open('ext.txt','rb').read().decode('u16'),chs)
print('Total chars:',len(chs))
chs=RandomIt(chs)
prechars=ReadPreChars()
print('generating map table...')
qt,qt2,et=GenerateMapTable(chs,prechars)

fs=open('qt1.tbl','wb')
for c in qt:
    fs.write(struct.pack('H',c))
fs.close()

fs=open('qt2.tbl','wb')
for c in qt2:
    fs.write(struct.pack('H',c))
fs.close()

pickle.dump(et,open('et.tbl','wb'))

for f in os.listdir(txtpath):
    if f.lower().endswith('.txt') and f!='yst00069.txt':
        stm=open(os.path.join(txtpath,f),'rb').read().decode('u16')
        print('converting '+f)
        ns=ReplaceText(stm,et)
        open(os.path.join(cvtpath,f),'wb').write(ns.encode('u16'))
        
stm=open(os.path.join(txtpath,'yst00069.txt'),'rb').read().decode('u16')
ns=ReplaceYurisNames(stm,et)
open(os.path.join(cvtpath,'yst00069.txt'),'wb').write(ns.encode('u16'))
