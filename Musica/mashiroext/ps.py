import struct
from bytefile import ByteIO

def exidx(stm):
    cnt=stm.readu32()
    ents=[]
    for i in range(cnt):
        name=stm.readstr().decode('932')
        ent=struct.unpack('I'*6,stm.read(24))
        ents.append([name]+list(ent))
    return ents

fs=open('a.dat','rb')
stm=ByteIO(fs.read())

entries=exidx(stm)

txt=[]
for ent in entries:
    txt.append(ent[0])

fs=open('a.txt','wb')
fs.write('\r\n'.join(txt).encode('U16'))
fs.close()
