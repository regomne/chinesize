def ext_ptr(ptr, txt):
  cnt=int.from_bytes(ptr[4:8], 'little')
  i=16
  ls=[]
  idx=0
  while i<len(ptr):
    num=int.from_bytes(ptr[i:i+4], 'little')
    off=int.from_bytes(ptr[i+4:i+8], 'little')
    size=int.from_bytes(ptr[i+8:i+12], 'little')
    ls.append('%05d:'%num+txt[off:off+size].decode('utf-8'))
    i+=12
    idx+=1
  return ls

def ext_file(ptrname, txtname, newname):
  fs=open(ptrname,'rb')
  ptr=fs.read()
  fs.close()
  fs=open(txtname,'rb')
  txt=fs.read()
  fs.close()
  ls=ext_ptr(ptr,txt)
  fs=open(newname,'wb')
  fs.write('\r\n'.join(ls).encode('u16'))
  fs.close()

path1=r'e:\Game\kamishippo\data\\'
path2=r'e:\Game\kamishippo\txt\\'

for i in range(14):
  base_name = 'ACT_'+chr(0x41+i)+'_JA'
  print('ext', base_name, '...')
  ext_file(path1+base_name+'.ptr',
    path1+base_name+'.txd',
    path2+base_name+'.txt')
