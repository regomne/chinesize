from struct import unpack
from difflib import SequenceMatcher
import os


def extIndexNames(fs):
  fs.seek(0x48)
  index_size, = unpack('Q', fs.read(8))
  stm = fs.read(index_size)
  file_cnt, = unpack('I', stm[0:4])
  off = 4
  file_infos = []
  for i in range(file_cnt):
    size, = unpack('I', stm[off:off+4])
    off += 0x20
    extra_len, = unpack('I', stm[off:off+4])
    off += 4+extra_len
    name_len, = unpack('I', stm[off:off+4])
    off += 4
    name = stm[off:off+name_len].rstrip(b'\0')
    off += name_len
    file_infos.append((name, size))
  return file_infos


def similar(a, b):
  return SequenceMatcher(None, a, b).ratio()


def findMaxSim(path, name, dic):
  max_sim = (0, '')
  fsize = os.path.getsize(os.path.join(path, name))
  for n in dic:
    newname, size = dic[n]
    if fsize == size:
      s = similar(name, n)
      if max_sim[0] < s:
        max_sim = (s, newname)
  if max_sim[0] == 0:
    raise Exception("error!")
  return max_sim[1]


def renameFiles(folder, infos):
  dic = {}
  for n, s in infos:
    dic[n.decode('932', 'ignore')] = (n.decode('utf-8'), s)
  for f in os.listdir(folder):
    if f in dic:
      newf, _ = dic[f]
      os.rename(os.path.join(folder, f), os.path.join(folder, newf))
    else:
      newf = findMaxSim(folder, f, dic)
      os.rename(os.path.join(folder, f), os.path.join(folder, newf))


fs = open(r'e:\Game\Steady2Steady\script.noa', 'rb')
infos = extIndexNames(fs)
fs.close()
#for n,s in infos:
#print(n.decode('932'),s)
renameFiles(r'e:\Game\Steady2Steady\script', infos)
