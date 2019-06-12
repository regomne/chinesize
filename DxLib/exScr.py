import os

def has_full(s):
  for c in s:
    if ord(c) > 0x80:
      return True
  return False

def ext_scr(ls):
  nl = []
  for l in ls:
    if has_full(l) and ('"' not in l or 'ruby text' in l) and (not l.lstrip().startswith('//')):
      nl.append(l)
  return nl

def ext_file(old, new):
  fs = open(old, 'rb')
  ls = fs.read().decode('932').split('\r\n')
  fs.close()
  nl = ext_scr(ls)
  fs = open(new, 'wb')
  fs.write('\r\n'.join(nl).encode('utf-8'))
  fs.close()

path1 = 'scripts'
path2 = 'txt'
for f in os.listdir(path1):
  ext_file(os.path.join(path1,f),
           os.path.join(path2,f))