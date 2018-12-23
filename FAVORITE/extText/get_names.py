import re

NamesMaxCnt = 100


def getnames(ls):
  pat_func = re.compile(r'Func.*\(([0-9A-F]+)\)')
  pat_pushstr = re.compile(r'.*PushStr (.*)')

  func_cnt = 0
  i = 0
  out = []
  while func_cnt < NamesMaxCnt:
    while not ls[i].startswith('Func'):
     i += 1
     if i >= len(ls):
       break
    mo = pat_func.match(ls[i])
    if not mo:
      raise Exception("not a valid function")

    func_addr = mo.group(1)
    name_str = ''

    i += 1
    while not ls[i].startswith('Func'):
      mo = pat_pushstr.match(ls[i])
      if mo:
        name_str = mo.group(1)
      i += 1

    func_cnt += 1

    out.append('    0x%s:"%s",' % (func_addr, name_str.strip(' 　')))

  return out

fs=open('world1.txt','rb')
ls = fs.read().decode('u16').split('\r\n')
out = getnames(ls)
for l in out: print(l)
