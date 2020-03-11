#encoding=utf-8
import os
import struct
import re

import bytefile

def parse_cmd(cmd):
  op = cmd & 0xffff
  int_cnt = (cmd & 0xff0000) >> 16
  str_cnt = (cmd >> 24) & 0xf
  unk = cmd >> 28
  return (op, int_cnt, str_cnt, unk)

def parse_name_table(ls):
  names = {}
  pat = re.compile(r'(\d+),\d*,\d*,([^,]+),.*')
  for l in ls:
    mo = pat.match(l)
    if mo:
      names[int(mo.group(1))] = mo.group(2)
  return names

def is_half(s):
  return len(s.decode('932'))==len(s)

def parse_rld_header(stm):
  magic, unk1, unk2, inst_cnt, unk3 = struct.unpack('<4sIIII', stm.read(20))
  tag = stm.readstr()
  stm.seek(0x114)
  return (magic, unk1, unk2, inst_cnt, unk3, tag)

def parse_rld(stm, name_table, cp):
  txt = []
  pure_txt = []
  magic, h_unk1, h_unk2, inst_cnt, h_unk3, h_tag = parse_rld_header(stm)
  
  if magic != b'\0DLR':
    raise Exception('error format!')
  
  txt.append('magic:%s, h_unk1:%d, h_unk2:%d, inst_cnt:%d,\
h_unk3:%d, h_tag:%s'%(magic, h_unk1, h_unk2, inst_cnt, h_unk3, h_tag))
  txt.append('')

  for i in range(inst_cnt):
    op, int_cnt, str_cnt, unk = parse_cmd(stm.readu32())
    txt.append('op:%d, int_cnt:%d, str_cnt:%d, unk:%d'%(op, int_cnt, str_cnt, unk))
    line = 'int:'
    ints = []
    for i in range(int_cnt):
      val = stm.readu32()
      line += ' %d,'%val
      ints.append(val)
    txt.append('\t'+line);
    strs = []
    for i in range(str_cnt):
      strs.append(stm.readstr())

    for s in strs:
      txt.append('\t'+s.decode(cp))
      
    if op == 28:
      if ints[0] in name_table:
        pure_txt.append('$' + name_table[ints[0]])
      for s in strs:
        if s != b'*' and s != b'$noname$' and len(s) != 0 and s.count(b',') < 2:
          pure_txt.append(s.decode(cp))
    elif op == 21:
      for s in strs:
        if s != b'*' and s != b'$noname$' and len(s) != 0 and s.count(b',') < 2:
          pure_txt.append(s.decode(cp))
    elif op == 48:
      pure_txt.append(strs[0].decode(cp))
    elif op == 191:
      s = strs[0]
      if not is_half(s):
        pure_txt.append(s.decode(cp))
    elif op == 203:
      s = strs[0]
      if not is_half(s):
        pure_txt.append(s.decode(cp))
  return txt, pure_txt

def write_txt(name,txt):
  fs=open(name,'wb')
  ntxt=[l.replace('\n','#n') for l in txt]
  fs.write('\r\n'.join(ntxt).encode('u16'))
  fs.close()

def ext_rld(fname1, fname2, name_table):
  fs=open(fname1, 'rb')
  stm = bytefile.ByteFile(fs.read())
  fs.close()
  try:
    txt, pure_txt = parse_rld(stm, name_table, '932')
  except:
    print(fname1, 'format error')
    return
  if len(pure_txt) > 0:
    write_txt(fname2, pure_txt)
    
def ext_all_rld(path1, path2):
  charfile = os.path.join(path1, 'defChara.rld')
  fs=open(charfile, 'rb')
  stm = bytefile.ByteFile(fs.read())
  fs.close()
  txt, pure_txt = parse_rld(stm, {}, '932')
  name_table = parse_name_table(pure_txt)
  for f in os.listdir(path1):
    if not f.endswith('.rld'):
      continue
    ext_rld(os.path.join(path1,f),
            os.path.join(path2,f.replace('.rld','.txt')),
            name_table)

ext_all_rld(r'e:\BaiduDownload\SexOpenWorld\rld2',
            r'e:\BaiduDownload\SexOpenWorld\txt')

