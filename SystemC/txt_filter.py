import os

path1='txt'
path2='txt2'

for f in os.listdir(path1):
  fs=open(os.path.join(path1,f),'rb')
  ls=fs.read().decode('u16').split('\r\n')
  fs.close()

  nl = []
  for l in ls:
    l=l[6:]
    if l.startswith(','):
      l=l[1:]
    elif l.startswith('＊,'):
      l=l[2:]
    nl.append(l)
  fs=open(os.path.join(path2,f),'wb')
  fs.write('\r\n'.join(nl).encode('u16'))
  fs.close()