import os
import sys


def has_full(s):
  for c in s:
    if ord(c) > 0x80:
      return True
  return False


def pack_scr(ori_ls, txt):
  nl = []
  tidx = 0
  for l in ori_ls:
    if has_full(l) and ('"' not in l or 'ruby text' in l) and\
       (not l.lstrip().startswith('//')):
      nl.append(txt[tidx])
      tidx += 1
    else:
      nl.append(l)
  return nl


def pack_file(oldfile, txtfile, newfile):
  fs = open(oldfile, 'rb')
  ls = fs.read().decode('932').split('\r\n')
  fs.close()
  fs = open(txtfile, 'rb')
  txt = fs.read().decode('932').split('\r\n')
  fs.close()
  try:
    nl = pack_scr(ls, txt)
  except:
    print('error packing:', newfile)
    return
  fs = open(newfile, 'wb')
  fs.write('\r\n'.join(nl).encode('932'))
  fs.close()


if len(sys.argv)<4:
  print('usage: %s <ori_script> <txt> <new_script>'%sys.argv[0])
  exit(0)

path1 = sys.argv[1]
path2 = sys.argv[2]
path3 = sys.argv[3]
for f in os.listdir(path1):
  if not os.path.isfile(os.path.join(path2, f)):
    continue
  pack_file(os.path.join(path1, f),
            os.path.join(path2, f),
            os.path.join(path3, f))
