import toml
import struct
import sys


def getstr(stm):
  p = stm.find(0)
  if p == -1:
    raise Exception('not a string')
  return stm[0:p]


def read_str_tbl(stm):
  txts = []
  cur = 0
  while cur < len(stm):
    s = getstr(stm[cur:])
    txts.append(s)
    cur += len(s)+1
  return txts


def pack_str_tbl(ls, cp):
  return b''.join([l.replace('\u30fb', '·').encode(cp) for l in ls])


def decode_entry(stm, pat, ext_idx):
  cur = 0
  txts = []
  pat_size = struct.calcsize(pat)
  while cur < len(stm):
    vals = struct.unpack(pat, stm[cur:cur+pat_size])
    for i in ext_idx:
      txts.append(getstr(vals[i]))
    cur += pat_size
  return txts


def encode_entry(stm, off, size, pat, ext_idx, ls, cp):
  cur = off
  lsidx = 0
  pat_size = struct.calcsize(pat)
  while cur < off+size:
    vals = struct.unpack(pat, stm[cur:cur+pat_size])
    if lsidx+len(ext_idx)-1 >= len(ls):
      raise Exception('line not enough')
    for i in ext_idx:
      ns = ls[lsidx+i].replace('\u30fb', '·').encode(cp)+b'\0'
      ns_len = len(vals[i])
      vals[i] = ns.ljust(ns_len, b' ')
    stm[cur:cur+pat_size] = struct.pack(pat, *vals)
    lsidx += len(ext_idx)
    cur += pat_size
  return lsidx


def decode_list(stm, desc):
  magic, filesize = struct.unpack('<4sI', stm[0:8])
  if magic != b'LIST' or filesize+8 != len(stm):
    raise Exception('stm header error')
  cur = 8
  texts = []
  while cur < len(stm):
    idx, blocksize = struct.unpack('<iI', stm[cur:cur+8])
    cur += 8
    if idx == -1:
      texts += read_str_tbl(stm[cur:cur+blocksize])
    elif str(idx) in desc:
      d = desc[str(idx)]
      texts += decode_entry(stm[cur:cur+blocksize], d['pat'], d['text'])
    cur += blocksize
  return texts


def encode_list(stm, desc, ls):
  magic, filesize = struct.unpack('<4sI', stm[0:8])
  if magic != b'LIST' or filesize+8 != len(stm):
    raise Exception('stm header error')
  cur = 8
  lsidx = 0
  has_str_tbl = False
  blocksize = 0
  while cur < len(stm):
    idx, blocksize = struct.unpack('<iI', stm[cur:cur+8])
    cur += 8
    if idx == -1:
      has_str_tbl = True
      break
    elif str(idx) in desc:
      d = desc[str(idx)]
      lsidx += encode_entry(stm, cur, blocksize,
                            d['pat'], d['text'], ls[lsidx:], desc['encoding'])
  if cur+blocksize != len(stm):
    raise Exception('strtbl not in the last of the file')
  line_cnt = len(read_str_tbl(stm[cur:]))
  if lsidx+line_cnt > len(ls):
    raise Exception('line not enough')
  strtbl = pack_str_tbl(ls[lsidx:lsidx+line_cnt])
  stm[cur:] = strtbl
  stm[cur-4:cur] = struct.pack('I', len(strtbl))
  stm[4:8] = struct.pack('I', len(stm)-8)
  return stm


def is_int(s):
  try:
    int(s)
  except:
    return False
  return True


def normalize_desc(desc):
  if 'encoding' not in desc:
    desc['encoding'] = '932'
  for i in desc:
    if is_int(i):
      d = desc[i]
      if not ('pat' in d and 'text' in d):
        raise Exception('descript file error')
  return desc


def ext_list_bin(fname, desc_name, newname):
  fs = open(fname, 'rb')
  stm = fs.read()
  fs.close()
  desc = normalize_desc(toml.load(desc_name))
  txts = decode_list(stm, desc)
  fs = open(newname, 'wb')
  fs.write('\r\n'.join([l.decode(desc['encoding'])
                        for l in txts]).encode('utf-8-sig'))
  fs.close()


def pack_list_bin(fname, desc_name, txtname, newname):
  fs = open(fname, 'rb')
  stm = fs.read()
  fs.close()
  desc = normalize_desc(toml.load(desc_name))
  fs = open(txtname, 'rb')
  txts = fs.read().decode('utf-8-sig').split('\r\n')
  fs.close()
  nstm = encode_list(stm, desc, txts)
  fs = open(newname, 'wb')
  fs.write(nstm)
  fs.close()

ext_list_bin('0db_scr.bin', 'scr.toml', 'scr.txt')
pack_list_bin('0db_scr.bin','scr.toml','scr.txt','db_scr.bin')
