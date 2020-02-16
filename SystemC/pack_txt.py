import os
import sys

def pack_txt(ori, txt):
  nt=[]
  ti=0
  for l in ori:
    prefix=l[:6]
    l=l[6:]
    if l.startswith(','):
      nt.append(prefix+','+txt[ti])
    elif l.startswith('＊,'):
      nt.append(prefix+'＊,'+txt[ti])
    else:
      nt.append(prefix+txt[ti])
    ti+=1
  return nt

path1=sys.argv[1]
path2=sys.argv[2]
path3=sys.argv[3]

for f in os.listdir(path1):
  path_txt=os.path.join(path2,f)
  if not os.path.exists(path_txt):
    continue
  fs=open(os.path.join(path1,f), 'rb')
  ori = fs.read().decode('u16').split('\r\n')
  fs.close()
  
  fs=open(path_txt, 'rb')
  txt=fs.read().decode('u16').split('\r\n')
  fs.close()

  try:
    nt = pack_txt(ori, txt)
  except Exception as e:
    print("error when process %s:"%f, e)
    continue

  fs=open(os.path.join(path3, f), 'wb')
  fs.write('\r\n'.join(nt).encode('u16'))
  fs.close()
