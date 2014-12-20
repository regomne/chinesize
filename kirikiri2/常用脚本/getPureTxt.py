#encoding=utf-8
import os
import re

path1='myks2'
path2='txt2'

patSel=re.compile(r'@exlink txt="([^"]*)".*')
patName=re.compile(r'@nm t="([^"]*)".*')
patParaBegin=re.compile(r'\*p\d+.*')

def isAllHalf(l):
    for c in l:
        if ord(c)>=0x100:
            return False
    return True
def ExtractPureTxt(lines):
    newtxt=[]
    i=0
    st='init'
    while i<len(lines):
        l=lines[i].strip('\t')
        if st=='init':
            mo=patSel.match(l)
            if mo:
                newtxt.append(mo.group(1))
                i+=1
                continue
            mo=patParaBegin.match(l)
            if mo:
                st='para'
                i+=1
                continue
        elif st=='para':
            mo=patName.match(l)
            if mo:
                newtxt.append(mo.group(1))
                i+=1
                continue
            if l.startswith('@nm_名前'):
                newtxt.append('【男主】')
                i+=1
                continue
            if l.startswith('@') or l.startswith('*'):
                i+=1
                continue
            if l.endswith('[np]'):
                newtxt.append(l[0:-4])
                st='init'
                i+=1
                continue
            if isAllHalf(l):
                i+=1
                continue
            newtxt.append(l)
        i+=1
    return newtxt


for f in os.listdir(path1):
    if not f.endswith('.ks'): continue

    fs=open(os.path.join(path1,f),'rb')
    lines=fs.read().decode('U16')
    if len(lines.replace('\r\n','\n').split('\n'))!=len(lines.split('\r\n')):
        print(f, ' not all \\r\\n')
    lines=lines.replace('\r\n','\n').split('\n')
    try:
        newtxt=ExtractPureTxt(lines)
    except Exception as e:
        print(f,e)
        continue

    fs=open(os.path.join(path2,f.replace('.ks','.txt')),'wb')
    fs.write('\r\n'.join(newtxt).encode('U16'))
    fs.close()
            

