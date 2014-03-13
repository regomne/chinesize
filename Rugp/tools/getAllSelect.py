import os
import struct
import re
#get all tag and sel: in txts

pat=re.compile(r'\d+\.txt')

path='txt'

def CalcHash(stm):
    h=0
    for c in stm:
        h+=(h<<5)+c
    return h&0xffffffff

sels=[]
hashs=[]
i=0
for f in os.listdir(path):
    if not pat.match(f):
        continue

    ls=open(os.path.join(path,f),'rb').read().decode('936').split('\r\n')
    for l in ls:
        if l.startswith('Sel:') or l.startswith('Tag:'):
            l=l[4:]

##            h=CalcHash(l.encode('932'))
##            if (not l in sels) and h in hashs:
##                print('rhash '+l)
##            hashs.append((h,i))
            sels.append(l)
            i+=1


hashs.sort()

fs=open(os.path.join(path,'sels.txt'),'wb')
fs.write('\r\n'.join(sels).encode('936'))
fs.close()
##fs=open(os.path.join(path,'selhash.tbl'),'wb')
##for h,i in hashs:
##    fs.write(struct.pack('II',h,i))
##fs.close()
