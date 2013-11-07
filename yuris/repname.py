import os

path1='ysbint'
path2='nt'

def ReadNames(fname):
    lines=open(fname,'rb').read().decode('u16').split('\r\n')
    names={}
    i=0
    status=0
    n=""
    while i<len(lines):
        l=lines[i]
        if len(l)==0:
            i+=1
            continue
        names[lines[i+1].strip('"')]=l.strip('"')
        i+=3

    names['爷A']='爷Ａ'
    names['爷B']='爷Ｂ'
    names['爷C']='爷Ｃ'
    return names

def RepTxt(lines,names):
    for i in range(len(lines)):
        l=lines[i]
        for key in names:
            if l.startswith(key):
                lines[i]=names[key]+l[len(key):]
                break
    return lines

ns=ReadNames('name.txt')
for f in os.listdir(path1):
    fs=open(os.path.join(path1,f),'rb')
    lines=fs.read().decode('u16').split('\r\n')
    ls=RepTxt(lines,ns)
    fs=open(os.path.join(path2,f),'wb')
    fs.write('\r\n'.join(ls).encode('u16'))
    fs.close()
