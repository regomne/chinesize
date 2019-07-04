import os


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
  txt = fs.read().decode('utf-8').split('\r\n')
  fs.close()
  nl = pack_scr(ls, txt)
  fs = open(newfile, 'wb')
  fs.write('\r\n'.join(nl).encode('932'))
  fs.close()


path1 = 'ori_scripts'
path2 = 'txt'
path3 = 'scripts'
for f in os.listdir(path1):
  if not os.path.isfile(os.path.join(path2, f)):
    continue
  pack_file(os.path.join(path1, f),
            os.path.join(path2, f),
            os.path.join(path3, f))
