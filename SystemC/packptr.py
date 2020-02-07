import struct

def pack_ptr(ls):
  ptr=[b'PTR ', len(ls).to_bytes(4,'little'), b'\0'*8]
  txd=[]
  txd_idx=0
  for l in ls:
    if l[5]!=':':
      raise Exception("line not illegal")
    num = int(l[0:5],10)
    s=l[6:].encode('utf-8')
    ptr.append(struct.pack('III', num, txd_idx, len(s)))
    txd.append(s)
    txd_idx+=len(s)
  return b''.join(ptr), b''.join(txd)

def pack_file(txtname, ptrname, txdname):
  fs=open(txtname,'rb')
  ls=fs.read().decode('u16').split('\r\n')
  fs.close()
  ptr,txd=pack_ptr(ls)
  fs=open(ptrname,'wb')
  fs.write(ptr)
  fs.close()
  fs=open(txdname,'wb')
  fs.write(txd)
  fs.close()

pack_file(r'e:\Game\kamishippo\txt\ACT_A_JA.txt',
r'e:\Game\kamishippo\patchxcn\ACT_A_JA.PTR',
r'e:\Game\kamishippo\patchxcn\ACT_A_JA.TXD')