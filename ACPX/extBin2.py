#encoding=utf-8
from struct import unpack,pack

def getstr(stm):
  p=stm.find(0)
  if p==-1:
    raise Exception('not a string')
  return stm[0:p]

def decode_name_t(stm):
  cur_off=0
  names=[]
  while cur_off<len(stm):
    text,color,pid,face=unpack('<64sII32s',stm[cur_off:cur_off+104])
    names.append(getstr(text).decode('932'))
    cur_off+=104
  return names

def decode_music_t(stm):
  cur_off=0
  music=[]
  while cur_off<len(stm):
    _,_,_,name,name_id=unpack('<64s64sH64sI',stm[cur_off:cur_off+198])
    music.append(getstr(name).decode('932'))
    cur_off+=198
  return music

def decode_hs_t(stm):
  cur_off=0
  music=[]
  while cur_off<len(stm):
    name1,name2,_=unpack('<64s64sH',stm[cur_off:cur_off+130])
    music.append(getstr(name1).decode('932'))
    music.append(getstr(name2).decode('932'))
    cur_off+=130
  return music

def decode_data_bin(fname):
  fs=open(fname,'rb')
  stm=fs.read()
  fs.close()
  magic,file_size=unpack('<4sI',stm[0:8])
  if magic!=b'LIST' or file_size+8!=len(stm):
    raise Exception('stm header err!')
  cur_off=8
  names=[]
  while cur_off<len(stm):
    idx,blocksize=unpack('<II',stm[cur_off:cur_off+8])
    cur_off+=8
    if idx==1:
      names=decode_name_t(stm[cur_off:cur_off+blocksize])
      
    cur_off+=blocksize

  return names

def decode_snd_bin(fname):
  fs=open(fname,'rb')
  stm=fs.read()
  fs.close()
  magic,file_size=unpack('<4sI',stm[0:8])
  if magic!=b'LIST' or file_size+8!=len(stm):
    raise Exception('stm header err!')
  cur_off=8
  names=[]
  hnames=[]
  while cur_off<len(stm):
    idx,blocksize=unpack('<II',stm[cur_off:cur_off+8])
    cur_off+=8
    if idx==0:
      names=decode_music_t(stm[cur_off:cur_off+blocksize])
    elif idx==2:
      hnames=decode_hs_t(stm[cur_off:cur_off+blocksize])
    cur_off+=blocksize

  return names,hnames

def write_txt(fname,strings):
  fs=open(fname,'wb')
  fs.write('\r\n'.join(strings).encode('u16'))
  #fs.write(b'\r\n'.join(strings))
  fs.close()

def read_txt(fname):
  fs=open(fname,'rb')
  ls = fs.read().decode('u16').split('\r\n')
  fs.close()
  return ls
  
def ext_data_bin(binfile,txtfile):
  names=decode_data_bin(binfile)
  write_txt(txtfile,names)
  
def ext_snd_bin(binfile,txtfile,txt2):
  names,hnames=decode_snd_bin(binfile)
  write_txt(txtfile,names)
  write_txt(txt2,hnames)

def pack_name_t(stm,off,size,ls):
  cur_off=off
  music=[]
  lsidx=0
  while cur_off<off+size:
    text,color,pid,face=unpack('<64sII32s',stm[cur_off:cur_off+104])
    if lsidx>=len(ls):
      raise Exception('line exceed')
    ns=ls[lsidx].replace('\u30fb','·').encode('936')+b'\0'
    ns=ns.ljust(64,b' ')
    stm[cur_off:cur_off+104] = pack('<64sII32s',ns,color,pid,face)
    lsidx+=1
    cur_off+=104
    
def pack_music_t(stm,off,size,ls):
  cur_off=off
  music=[]
  lsidx=0
  while cur_off<off+size:
    a1,a2,a3,name,a4=unpack('<64s64sH64sI',stm[cur_off:cur_off+198])
    if lsidx>=len(ls):
      raise Exception('line exceed')
    ns=ls[lsidx].replace('\u30fb','·').encode('936')+b'\0'
    ns=ns.ljust(64,b' ')
    stm[cur_off:cur_off+198] = pack('<64s64sH64sI',a1,a2,a3,ns,a4)
    lsidx+=1
    cur_off+=198
    
def pack_name(fname,ls):
  fs=open(fname,'rb')
  stm=bytearray(fs.read())
  fs.close()
  magic,file_size=unpack('<4sI',stm[0:8])
  if magic!=b'LIST' or file_size+8!=len(stm):
    raise Exception('stm header err!')
  cur_off=8
  while cur_off<len(stm):
    idx,blocksize=unpack('<II',stm[cur_off:cur_off+8])
    cur_off+=8
    if idx==1:
      pack_name_t(stm,cur_off,blocksize,ls)
    cur_off+=blocksize
  return stm

def pack_music(fname,ls):
  fs=open(fname,'rb')
  stm=bytearray(fs.read())
  fs.close()
  magic,file_size=unpack('<4sI',stm[0:8])
  if magic!=b'LIST' or file_size+8!=len(stm):
    raise Exception('stm header err!')
  cur_off=8
  while cur_off<len(stm):
    idx,blocksize=unpack('<II',stm[cur_off:cur_off+8])
    cur_off+=8
    if idx==0:
      pack_music_t(stm,cur_off,blocksize,ls)
    cur_off+=blocksize
  return stm

def pack_name_bin(fname,txtname,newname):
  ls=read_txt(txtname)
  stm=pack_name(fname,ls)
  fs=open(newname,'wb')
  fs.write(stm)
  fs.close()
  
def pack_music_bin(fname,txtname,newname):
  ls=read_txt(txtname)
  stm=pack_music(fname,ls)
  fs=open(newname,'wb')
  fs.write(stm)
  fs.close()
  
#ext_bin('data\\db_scr.bin','data\\scr.txt')
#ext_snd_bin('data\\db_snd.bin','data\\snd.txt','data\\hs.txt')
pack_name_bin('data\\db_scr0.bin','data\\scr1.txt','data\\db_scr.bin')
#pack_music_bin('data\\db_snd.bin','data\\snd.txt','data\\db_snd1.bin')
