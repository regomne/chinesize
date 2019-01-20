import os
import struct
import zlib

def excst(fs):
  fs.seek(0x34)
  raw=fs.read()
  ori=zlib.decompress(raw)
  return ori

path1='kcs'
path2='kcs_ext'
if not os.path.exists(path2):
  os.makedirs(path2)
for f in os.listdir(path1):
  fs=open(os.path.join(path1,f),'rb')
  newf = excst(fs)
  fs.close()
  fs=open(os.path.join(path2,f),'wb')
  fs.write(newf)
  fs.close()
