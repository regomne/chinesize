import os
import struct
import sys

def calc_hash(name):
  name=name.upper().encode('utf-8')
  h=0
  for i in range(len(name)):
    h=(h+name[i]*(i+1))&0xffff
  return h

def pack(folder, output):
  files = os.listdir(folder)
  files.sort(key=calc_hash)
  fs= open(output, 'wb')
  fs.write((len(files)|0x80000000).to_bytes(4,'little'))
  idx = bytearray()
  off = 4
  for f in files:
    ff=open(os.path.join(folder,f),'rb')
    stm = ff.read()
    fs.write(stm)
    ff.close()
    idx+=struct.pack('II24sI', off, len(stm), f.encode('utf-8'), calc_hash(f))
    off+=len(stm)
  fs.write(idx)
  fs.write((0).to_bytes(4,'little'))
  fs.write(off.to_bytes(4, 'little'))
  fs.close()

pack(sys.argv[1],
  sys.argv[2])