#encoding=utf-8
import os
from pdb import set_trace as int3

path='mest'

def GetArasuji(fname):
    fs=open(fname,'rb')
    lines=fs.read().decode('U16').split('\r\n')
    fs.close()
    i=0
    while i<len(lines):
        l=lines[i].lstrip('　')
        if l.startswith('●'):
            ln=lines[i+1].lstrip('　')
            if (ln!='是') and (ln[1]!='是'):
                print(fname)
                int3()
            return lines[i-1]
        i+=1
    print(fname+' not find')
    return ''

txt=[]
for f in os.listdir(path):
    if (ord(f[0])<ord('0')) or (ord(f[0])>ord('9')):
        continue
    s=GetArasuji(os.path.join(path,f))
    if s!='':
        txt.append(s)

fs=open('meses.txt','wb')
fs.write('\r\n'.join(txt).encode('U16'))
fs.close()
