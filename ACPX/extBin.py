#encoding=utf-8
import os
import struct
from pdb import set_trace as int3

halfgana='!?｡｢｣､･ｦｧｨｩｪｫｬｭｮｯｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ'
fullgana='！？　。「」、…をぁぃぅぇぉゃゅょっーあいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわん゛゜'
Ganatbl={}
for i in range(len(halfgana)):
  Ganatbl[halfgana[i]]=fullgana[i]
NameTable=[]

def get_str_from_bin(stm,off):
  p=stm.find(b'\0',off)
  if p==-1:
    raise Exception('not find \\0')
  return stm[off:p]

BasicCodeLengthTbl=[
  0, 4, 4, 4, 0, 4, 0, 4, #0-7
  0, 0, 0, 0, 0, 0, 0, 0, #8-F
  0, 0, 0, 0, 0, 0, 0, 0, #10-17
  0, 0, 0, 0, 0, 0, 0, 0, #18-1F
  0, 4
  ]

ScriptCmdLengthTbl=[
  0, 0, 0, 0, 0, 0, 0, 0, #22
  0, 4, 0, 0, 0, 4, 0, 4, #2A
  4, 4, 0, 0, 0, 0, 0, 0, #32
  4, 0, 0, 0, 0, 0, 0, 0, #3A
  0, 0, 0, 0, 0, 0, 0, 0, #42
  0, 0, 0, 0, 0, 0, 0, 0, #4A
  0, 0, 0, 0, 0, 0, 0, 0, #52
  0, 0, 0, 0, 0, 0, 0, 0, #5A
  0, 0, 0, 0, 0, 4, 0, 0, #62
  0, 0, 0, 0, 0, 0, 0, 0, #6A
  0,                      #72
  ]

def read_u32(stm,p):
  return struct.unpack('I',stm[p:p+4])[0]

def is_all_type(arr,tp):
  for tp1,_ in arr:
    if tp1!=tp:
      return False
  return True

def count_if(arr,func):
  cnt=0
  for a in arr:
    if func(a):
      cnt+=1
  return cnt

def parse_code(code):
  cur_off=0
  stack=[]
  p_status='other' #'push'
  stridx=[]
  while cur_off<len(code):
    c=code[cur_off]
    cur_off+=1
    step=-1
    if c<0x22:
      step=BasicCodeLengthTbl[c]
    elif c<=0x72:
      step=ScriptCmdLengthTbl[c-0x22]
    else:
      raise Exception('code error!')

    if c==5: #push
      if p_status=='other':
        p_status='push'
      stack.append(('push',read_u32(code,cur_off)))
    elif p_status=='push':
        stack.append(('other',0))
        p_status='other'

    if c==7: #str
      stack.append(('str',read_u32(code,cur_off)))
    elif c==0x2a: #MES
      tp,strid=stack[-1]
      if tp!='str':
        raise Exception('code:%x shoud be str'%cur_off)
      stridx.append(('mes',strid))
    elif c==0x2b: #TLK
      param_cnt=read_u32(code,cur_off)
      if is_all_type(stack[-param_cnt-1:-1],'push'):
        stridx.append(('name',stack[-param_cnt-1][1]))
      else:
        #int3()
        print('not all push name: %x'%cur_off)
        stridx.append(('name',-1))
    elif c==0x2c: #MENU
      #if is_all_type(stack[-4:-1],'push'):
      if stack[-3][0] == 'str':
        stridx.append(('menu',stack[-3][1]))
      else:
        print('not all push menu: %x'%cur_off)
        stridx.append(('menu',-1))
    elif c==0x5f: #TITLE
      if stack[-4][0]!='str':
        print('info error:'%cur_off)
        stridx.append(('info',-1))
      else:
        stridx.append(('info',stack[-4][1]))
    cur_off+=step
  return stridx

def gen_txt_from_stridx(stridx,strs):
  txt=[]
  for tp,idx in stridx:
    if tp=='mes':
      txt.append(strs[idx])
    elif tp=='name':
      if idx>=0:
        name=NameTable[idx]
      else:
        name='unk'
      txt.append('$'+name)
    elif tp=='menu':
      if idx>=0:
        menu=strs[idx]
      else:
        menu='unk'
      #print('menu:',menu)
      txt.append('>'+menu)
    elif tp=='info':
      if idx>=0:
        info=strs[idx]
      else:
        info='unk'
      txt.append(info)
  return txt

def read_bin(fname):
  fs=open(fname,'rb')
  stm=fs.read()
  fs.close()

  if stm[0:8]!=b'ESCR1_00':
    raise Exception(fname+'not escr')
  
  str_cnt,=struct.unpack('I',stm[8:12])
  cur_off=12+str_cnt*4
  str_offs=struct.unpack('I'*str_cnt,stm[12:cur_off])

  code_size,=struct.unpack('I',stm[cur_off:cur_off+4])
  cur_off+=4

  code=stm[cur_off:cur_off+code_size]
  cur_off+=code_size
  string_size,=struct.unpack('I',stm[cur_off:cur_off+4])
  cur_off+=4
  if cur_off+string_size != len(stm):
    raise Exception('string size err')
  strings=[]
  for off in str_offs:
    s=get_str_from_bin(stm,cur_off+off)
    s=s.decode('932')
    ns=[]
    for c in s:
      if c in Ganatbl:
        ns.append(Ganatbl[c])
      else:
        ns.append(c)
    strings.append(''.join(ns))
    #strings.append(s)

  return (code,strings)

def write_txt(fname,strings):
  fs=open(fname,'wb')
  fs.write('\r\n'.join(strings).encode('u16'))
  #fs.write(b'\r\n'.join(strings))
  fs.close()

def read_txt(fname):
  fs=open(fname,'rb')
  return fs.read().decode('u16').split('\r\n')

def ext_bin(binfile,txtfile):
  code,strs=read_bin(binfile)
  write_txt(txtfile,strs)

def ext_bin2(binfile,txtfile):
  code,strs=read_bin(binfile)
  stridx=parse_code(code)
  nstr_cnt=count_if(stridx,lambda x: x[0]!='name')
  if len(strs)-1!=nstr_cnt:
    print('str:',len(strs),'nstr:',nstr_cnt)
  txt=gen_txt_from_stridx(stridx,strs)
  write_txt(txtfile,txt)

def extEscrAll(path1,path2,func):
  for f in os.listdir(path1):
    if not f.endswith('.bin'):
      continue
    print('extracting',f)
    func(os.path.join(path1,f),
            os.path.join(path2,f.replace('.bin','.txt')))

def gen_bin(txtls,code,str_cnt,cp):
  if len(txtls)!=str_cnt:
    print('txt line error')
    return b''
  cur_off=0
  offs=[]
  strls=[]
  for l in txtls:
    encoded=l.encode(cp,'replace')+b'\0'
    strls.append(encoded)
    offs.append(struct.pack('<I',cur_off))
    cur_off+=len(encoded)
  strings=b''.join(strls)
  hdr=b'ESCR1_00'
  stm=bytearray()
  stm+=hdr
  stm+=struct.pack('<I',str_cnt)
  stm+=b''.join(offs)
  stm+=struct.pack('<I',len(code))
  stm+=code
  stm+=struct.pack('<I',len(strings))
  stm+=strings
  return stm

def pack_bin(binfile,txtfile,newbinfile,cp):
  code,strings=read_bin(binfile)
  txtls=read_txt(txtfile)
  stm=gen_bin(txtls,code,len(strings),cp)
  fs=open(newbinfile,'wb')
  fs.write(stm)
  fs.close()

def read_name_file(fname):
  fs=open(fname,'rb')
  ls=fs.read().decode('u16').split('\r\n')
  fs.close()
  return ls

NameTable=read_name_file('data\\db_scr.txt')
extEscrAll('0script','txt',ext_bin2)
##pack_bin('script_ori\\01_01.bin',
##         'script_txt\\01_01.txt',
##         'script\\01_01.bin',
##         '936')
#ext_bin2('0script\\z_test.bin','test.txt')
