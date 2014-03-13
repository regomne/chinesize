import os
import struct

path1='txt'

hashes=[]
for f in os.listdir(path1):
    if f.endswith('.tbl'):
        ss=open(os.path.join(path1,f),'rb').read(8)
        h,=struct.unpack('I',ss[4:8])
        hashes.append((h,int(f[0:-4])))

hashes.sort()

c=bytearray()
for h,i in hashes:
    c+=struct.pack('II4s',h,i,b'\0'*4)

open('hash.tbl','wb').write(c)
