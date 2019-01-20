import os
import zlib
import struct

path1='fes_ext'
path2='fes'
for f in os.listdir(path1):
  if not f.endswith('.fes'):
    continue
  fs=open(os.path.join(path1,f),'rb')
  stm=fs.read()
  fs.close()
  ns=zlib.compress(stm)
  fs=open(os.path.join(path2,f),'wb')
  fs.write(struct.pack('<4sIII',b'FES\0',len(ns),len(stm),0))
  fs.write(ns)
  fs.close()
