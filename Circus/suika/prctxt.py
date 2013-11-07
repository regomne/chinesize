#encoding=utf-8
fs=open('new  3.txt','rb')
nlines=fs.read().decode('U16').split('\r\n')

fs=open('exe.txt','rb')
olines=fs.read().decode('U16').split('\r\n')

if len(nlines)!=len(olines):
    affd

nnl=[]

for i in range(len(nlines)):
    l1=olines[i]
    if l1[1]=='月':
        nl=l1[0:6]+nlines[i].lstrip('＜').rstrip('＞')
        nnl.append(nl)
    else:
        nnl.append(nlines[i])

fs=open('exen.txt','wb')
fs.write('\r\n'.join(nnl).encode('U16'))
fs.close()
