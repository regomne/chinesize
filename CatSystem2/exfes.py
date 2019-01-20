import os
import struct
import zlib

def excst(fs):
  fs.seek(4)
  raw_size, ori_size = struct.unpack('II',fs.read(8))
  fs.seek(4,1)
  raw=fs.read()
  if len(raw)!=raw_size:
    raise Exception('size error!')
  ori=zlib.decompress(raw)
  if len(ori)!=ori_size:
    raise Exception('size error2!')
  return ori

path1='fes0'
path2='fes_ext'
if not os.path.exists(path2):
  os.makedirs(path2)
for f in os.listdir(path1):
  fs=open(os.path.join(path1,f),'rb')
  newf = excst(fs)
  fs.close()
  fs=open(os.path.join(path2,f),'wb')
  fs.write(newf)
  fs.close()
