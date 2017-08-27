#encoding=utf8
import os
import struct
import shutil

import bytefile

MES_NAME_OP = 0x4d
MES_TEXT_OP = 0x4e
MES_VOICE1_OP = 0x36
MES_VOICE2_OP = 0x37
MES_SEL_OP = 8
MES_KNOWN_OPS = [0x04, 0x62, 0x17, 0x1a]
#1和2的header不同，参见read_mes
MesVersion = 1

def dec_str(s):
  ns=[]
  for c in s:
    ns.append((c+0x20)&0xff)
  return bytes(ns)

def enc_str(s):
  ns=[]
  for c in s:
    ns.append((c+0x100-0x20)&0xff)
  return bytes(ns)

def parse_indexes(s):
  cnt = int(len(s)/4)
  ids = []
  for i in range(cnt):
    ids.append(struct.unpack('I',s[i*4:i*4+4])[0])
  return ids

def pack_indexes(ind):
  return b''.join([struct.pack('<I',i) for i in ind])

def add_text(bs,txt_idxes,txts,cmd_start):
  pos = bs.tell()
  t = bs.readstr()
  txt_idxes.append((pos - cmd_start, len(t)))
  txts.append(dec_str(t))

def read_mes(bs):
  cmd_cnt = bs.readu32()
  indexes = parse_indexes(bs.read(cmd_cnt * 4))
  if MesVersion>=2:
    unk_indexes = bs.read(cmd_cnt * 2)
  cmd_start = bs.tell()
  txts = []
  txt_idxes = []
  for off in indexes:
    off &= 0x7fffffff
    bs.seek(cmd_start + off)
    op = bs.readu8()
    if op == MES_VOICE1_OP:
      bs.readstr()
      op = bs.readu8()
      if op == MES_VOICE2_OP:
        bs.readstr()
        op = bs.readu8()
      elif op == MES_NAME_OP or op == MES_TEXT_OP:
        pass
      else:
        print('op error voice2', hex(bs.tell()))
    elif op == MES_VOICE2_OP:
      bs.readstr()
      op = bs.readu8()
      
    if op == MES_NAME_OP:
      add_text(bs,txt_idxes,txts,cmd_start)
      op2 = bs.readu8()
      if op2 == MES_VOICE2_OP:
        bs.readstr()
        op2 = bs.readu8()
      if op2 == MES_TEXT_OP:
        add_text(bs,txt_idxes,txts,cmd_start)
      else:
        print('op error name', hex(bs.tell()))
    elif op == MES_TEXT_OP:
      add_text(bs,txt_idxes,txts,cmd_start)
    elif op == MES_SEL_OP:
      #print('read 8', hex(bs.tell))
      bs.readu16()
      while bs.readstr().startswith(b'D'):
        bs.readu8()
        add_text(bs,txt_idxes,txts,cmd_start)
    elif op not in MES_KNOWN_OPS:
      print('op error op', hex(op), hex(bs.tell()))
  return txts,txt_idxes,indexes

def split_block(stm,indexes):
  bks = []
  off = 0
  i = 0
  while i < len(indexes):
    n = indexes[i]
    bks.append(stm[off:n])
    off = n
    i += 1
  if off > len(stm):
    raise Exception('index out of range')
  bks.append(stm[off:])
  return bks

def split_2d_block(ind_blocks, txt_idxes):
  b_off = 0
  txt_idx = 0
  final_blocks = []
  new_2d_idxes = []
  for b_idx in range(len(ind_blocks)):
    indb = ind_blocks[b_idx]
    ni = []
    #print('ind:',b_idx,hex(b_off),hex(len(indb)))
    while txt_idx < len(txt_idxes):
      off, lens = txt_idxes[txt_idx]
      #print('txt:',txt_idx,hex(off),hex(lens))
      if off >= b_off:
        if off + lens < b_off + len(indb):
          new_2d_idxes.append((b_idx, len(ni) + 1))
          ni.append(off - b_off)
          ni.append(off + lens - b_off)
        else:
          break
      else:
        raise Exception('error split')
      txt_idx += 1
    final_blocks.append(split_block(indb,ni))
    b_off += len(indb)
  return final_blocks, new_2d_idxes

def pack_mes(bs, ntxt, txt_idxes, indexes):
  if len(ntxt)!=len(txt_idxes):
    raise Exception('len error!')
  cmd_cnt = bs.readu32()
  if cmd_cnt != len(indexes):
    raise Exception('len1 error!')

  has_high_bits = [i & 0x80000000 != 0 for i in indexes]
  low_inds = [i & 0x7fffffff for i in indexes]

  cmd_start = 4 + cmd_cnt * 4
  if MesVersion >=2:
    cmd_start += cmd_cnt * 2
  ind_blocks = split_block(bs[cmd_start:], low_inds)
  final_blocks, new_2d_idxes = split_2d_block(ind_blocks, txt_idxes)
  #print('len new 2d ind:',len(new_2d_idxes),'len txt',len(ntxt))
  for i in range(len(ntxt)):
    l = enc_str(ntxt[i].replace('\\n','\n').encode('936'))
    x, y = new_2d_idxes[i]
    final_blocks[x][y] = l
  ind_blocks = [b''.join(b) for b in final_blocks]
  new_indexes = []
  off = len(ind_blocks[0])
  i = 1
  while i < len(ind_blocks):
    val = off
    if has_high_bits[i - 1]:
      val |= 0x80000000
    new_indexes.append(val)
    off += len(ind_blocks[i])
    i += 1
  return bs[0:4] +\
    pack_indexes(new_indexes) +\
    bs[4 + cmd_cnt * 4: cmd_start] +\
    b''.join(ind_blocks)

def write_txts(fs,txts):
  nt=[t.decode('932').replace('\n','\\n') for t in txts]
  fs.write('\r\n'.join(nt).encode('u16'))

def ext_all_mes(pathori, pathtxt):
  for f in os.listdir(pathori):
    if not f.endswith('.mes'):
      continue
    print(f)
    fs=open(os.path.join(pathori,f),'rb')
    bs=bytefile.ByteFile(fs.read())
    fs.close()
    txts,txt_idxes,_ = read_mes(bs)
    if len(txts) != 0:
      fs=open(os.path.join(pathtxt,f.replace('.mes','.txt')),'wb')
      write_txts(fs,txts)
      fs.close()

def pack_all_mes(pathori,pathtxt,pathnew):
  for f in os.listdir(pathori):
    if not f.endswith('.mes'):
      continue
    txtname = f.replace('.mes','.txt')
    if not os.path.isfile(os.path.join(pathtxt,txtname)):
      shutil.copyfile(os.path.join(pathori,f),os.path.join(pathnew,f))
    else:
      #print(f)
      fs=open(os.path.join(pathori,f),'rb')
      bs=bytefile.ByteFile(fs.read())
      fs.close()
      txts, txt_idxes, indexes = read_mes(bs)
      fs=open(os.path.join(pathtxt,txtname),'rb')
      ls = fs.read().decode('u16').split('\r\n')
      fs.close()
      if ls[-1]=='':
        del ls[-1]
      if len(ls) != len(txt_idxes):
        print(txtname, 'line not fit')
        continue
      bs.seek(0)
      newmes = pack_mes(bs, ls, txt_idxes, indexes)
      fs=open(os.path.join(pathnew,f),'wb')
      fs.write(newmes)
      fs.close()

##ext_all_mes(r'D:\galgame\fortissimo FA\advdata\0MES',
##            r'D:\galgame\fortissimo FA\advdata\0txt')

pack_all_mes(r'D:\galgame\fortissimo FA\advdata\0MES',
             r'D:\galgame\fortissimo FA\advdata\txt',
             r'D:\galgame\fortissimo FA\advdata\MES')
