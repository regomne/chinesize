import os

ori_path = 'scenario'
new_path = 'txt'

def line_has_fullangle(l):
  for c in l:
    if ord(c) > 0x80:
      return True
  return False

def ext_s(ls):
  newl=[]
  for l in ls:
    l = l.strip()
    if len(l) == 0:
      continue
    if l[0] == '^':
      if l.startswith('^savescene,') or l.startswith('^select,'):
        newl.append(l)
    elif l[0] == '％' or l[0] == '\\':
      pass
    elif line_has_fullangle(l):
      newl.append(l)
  return newl

for root,dirs,files in os.walk(ori_path):
  rel = os.path.relpath(root, ori_path)
  nroot = os.path.join(new_path, rel)
  if not os.path.exists(nroot):
    os.makedirs(nroot)
  
  for f in files:
    print('processing', os.path.join(root, f))
    fs = open(os.path.join(root, f), 'rb')
    ls = fs.read().decode('u16').split('\r\n')
    fs.close()
    nl = ext_s(ls)
    if len(nl) != 0:
      fs = open(os.path.join(nroot, f.replace('.s','.txt')), 'wb')
      fs.write('\r\n'.join(nl).encode('u16'))
      fs.close()