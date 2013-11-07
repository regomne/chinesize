import struct

def CODE(ain):
  sp=ain.tell()
  codelen=ain.readu32()
  return (sp,codelen+4,ain.read(codelen))

def VERS(ain):
  ver=ain.readu32()
  return (ain.tell()-4,4,ver)

def FUNC(ain):
  start_pos=ain.tell()
  func_num=ain.readu32()
  funcs=[]
  for i in range(func_num):
    func_addr=ain.readu32()
    func_name=ain.readstr()
    func_type, func_ret0, func_ret1, func_argc, func_argvarc, func_unk1=struct.unpack('IIIIII',ain.read(24))
    vars=[]
    for j in range(func_argvarc):
      var_name=ain.readstr()
      type0, type1, type2=struct.unpack('III',ain.read(12))
      vars.append((var_name,type0,type1,type2))
    funcs.append((func_addr,func_name,func_type, func_ret0, func_ret1, \
      func_argc, func_argvarc, func_unk1, vars))
  funclen=ain.tell()-start_pos
  return (start_pos,funclen,funcs)

def fastFUNC(ain):
  start_pos=ain.tell()
  func_num=ain.readu32()
  for i in range(func_num):
    ain.seek(4,1)
    ain.seek(ain.find('\0',ain.tell())+1)
    ain.seek(16,1)
    func_argvarc=ain.readu32()
    ain.seek(4,1)
    for j in range(func_argvarc):
      ain.seek(ain.find('\0',ain.tell())+1)
      ain.seek(12,1)
  funclen=ain.tell()-start_pos
  return (start_pos,funclen,[])

def GLOB(ain):
  start_pos=ain.tell()
  vars=[]
  for i in range(ain.readu32()):
    var_name=ain.readstr()
    t0, t1, t2, unk1=struct.unpack('IIII',ain.read(16))
    vars.append((var_name,t0,t1,t2,unk1))
  nlen=ain.tell()-start_pos
  return (start_pos,nlen,vars)

def GSET(ain):
  sp=ain.tell()
  values=[]
  for i in range(ain.readu32()):
    var_num, t0=struct.unpack('II',ain.read(8))
    if t0==0xC:
      value=ain.readstr()
    else:
      value=ain.readu32()
    values.append((var_num,t0,value))
  nlen=ain.tell()-sp
  return (sp,nlen,values)

def STRT(ain):
  sp=ain.tell()
  structs=[]
  for i in range(ain.readu32()):
    strt_name=ain.readstr()
    constructor, destructor, elem_num=struct.unpack('III',ain.read(12))
    elems=[]
    for j in range(elem_num):
      elem_name=ain.readstr()
      t0, t1, t2=struct.unpack('III',ain.read(12))
      elems.append((elem_name,t0,t1,t2))
    structs.append((strt_name,constructor,destructor,elem_num,elems))
  nlen=ain.tell()-sp
  return (sp,nlen,structs)

def fastSTRT(ain):
  sp=ain.tell()
  for i in range(ain.readu32()):
    ain.seek(ain.find('\0',ain.tell())+1)
    ain.seek(8,1)
    elem_num=ain.readu32()
    for j in range(elem_num):
      ain.seek(ain.find('\0',ain.tell())+1)
      ain.seek(12,1)
  nlen=ain.tell()-sp
  return (sp,nlen,[])

def MSG0(ain):
  sp=ain.tell()
  msg=[]
  for i in range(ain.readu32()):
    msg.append(ain.readstr())
  nlen=ain.tell()-sp
  return (sp,nlen,msg)

def MAIN(ain):
  num=ain.readu32()
  return (ain.tell()-4,4,num)

def MSGF(ain):
  num=ain.readu32()
  return (ain.tell()-4,4,num)

def HLL0(ain):
  sp=ain.tell()
  hlls=[]
  for i in range(ain.readu32()):
    hll_name=ain.readstr()
    func_num=ain.readu32()
    funcs=[]
    for j in range(func_num):
      func_name=ain.readstr()
      func_ret, func_argc=struct.unpack('II',ain.read(8))
      args=[]
      for k in range(func_argc):
        arg_name=ain.readstr()
        arg_t0=ain.readu32()
        args.append((arg_name,arg_t0))
      funcs.append((func_name,func_ret,func_argc,args))
    hlls.append((hll_name,func_num,funcs))
  nlen=ain.tell()-sp
  return (sp,nlen,hlls)

def SWI0(ain):
  sp=ain.tell()
  switches=[]
  for i in range(ain.readu32()):
    t0, t1, num=struct.unpack('III',ain.read(12))
    cases=[]
    for j in range(num):
      cases.append(struct.unpack('II',ain.read(8)))
    switches.append((t0,t1,num,cases))
  nlen=ain.tell()-sp
  return (sp,nlen,switches)

def GVER(ain):
  sp=ain.tell()
  return (sp,4,ain.readu32())

def STR0(ain):
  sp=ain.tell()
  msg=[]
  for i in range(ain.readu32()):
    msg.append(ain.readstr())
  nlen=ain.tell()-sp
  return (sp,nlen,msg)

def FNAM(ain):
  sp=ain.tell()
  msg=[]
  for i in range(ain.readu32()):
    msg.append(ain.readstr())
  nlen=ain.tell()-sp
  return (sp,nlen,msg)

def OJMP(ain):
  return (ain.tell(),4,ain.readu32())

def FNCT(ain):
  nlen, fnct_num=struct.unpack('II',ain.read(8))
  fnct=[]
  for i in range(fnct_num):
    fnct_name=ain.readstr()
    fnct_ret0, fnct_ret1, fnct_argc, fnct_varargc=struct.unpack('IIII',ain.read(16))
    args=[]
    for j in range(fnct_varargc):
      name=ain.readstr()
      t0, t1, t2=struct.unpack('III',ain.read(12))
      args.append((name,t0,t1,t2))
    fnct.append((fnct_name,fnct_ret0,fnct_ret1,fnct_argc,fnct_varargc,args))
  return (ain.tell()-nlen,nlen,fnct)

def SLBL(ain):
  sp=ain.tell()
  slbls=[]
  for i in range(ain.readu32()):
    name=ain.readstr()
    slbls.append((name,ain.readu32()))
  return (sp,ain.tell()-sp,slbls)

def KEYC(ain):
  return (ain.tell(),4,ain.readu32())

def OBJG(ain):
  sp=ain.tell()
  msg=[]
  for i in range(ain.readu32()):
    msg.append(ain.readstr())
  nlen=ain.tell()-sp
  return (sp,nlen,msg)

AINFUNC={
  'CODE':CODE,
  'VERS':VERS,
  'FUNC':FUNC,
  'GLOB':GLOB,
  'GSET':GSET,
  'STRT':STRT,
  'MSG0':MSG0,
  'MAIN':MAIN,
  'MSGF':MSGF,
  'HLL0':HLL0,
  'SWI0':SWI0,
  'GVER':GVER,
  'SLBL':SLBL,
  'STR0':STR0,
  'FNAM':FNAM,
  'OJMP':OJMP,
  'FNCT':FNCT,
  'KEYC':KEYC,
  'OBJG':OBJG
}
