#encoding=utf-8
#py2.7
import strfile
from struct import unpack
import pdb

class NsbParser:
  "Parse the nsb file."
  def __init__(self,nsb):
    self.nsb=nsb
    self.text=[]
    self.stack=[]
    self.tabcount=0
    self.opcode=0
    self.argc=0
  def chkargc(self,num):
    if self.argc!=num: pdb.set_trace()
  def readarg(self):
    return self.nsb.read(self.nsb.readu32())
  def makearg(self,num):
    if num!=0:
      tbl='('+', '.join(self.stack[-num:])+')'
      del self.stack[-num:]
      for i in range(num):
        self.readarg()
      return tbl
    else:
      return '()'

  sysfunc=(
    0, 0, 0, 0, 0, 0, ('CreateStencil',0), 0,
    0, ('CreateProcess',0), ('CreateColor',0), ('CreateErrect',0), 0, ('CreateTexture',0), 0, 0,
    0, ('CreateSound',0), ('CreateMovie',0), ('CreateText',0), ('CreateWindow',0), 0, ('CreateChoice',0), 0,#10
    0, ('LoadImage',0), 0, 0, 0, ('LoadText',0), 0, 0,
    ('Move',0), 0, 0, ('Request',0), ('Delete',0), ('Fade',0), 0, 0,#20
    0, 0, 0, ('SetAlias',0), 0, 0, 0, 0, 
    0, 0, 0, 0, ('SetNextFocus',0), 0, 0, 0,#30
    ('DrawTransition',0), ('Wait',0), ('WaitKey',0), 0, 0, 0, 0, 0,
    ('WaitAction',0), ('WaitPlay',0), ('WaitText',0), ('SetVolume',0), ('SetPan',0), ('SetFrequency',0), ('SetLoop',0), ('SetLoopPoint',0),#40
    0, 0, ('ClearBacklog',0), 0, ('ClearScore',0), 0, ('SetFont',0), ('Save',0),
    0, ('DeleteSaveFile',0), ('ExistSave',1), 0, 0, 0, 0, 0,#50
    ('SetShortCut',0), ('Escape',0), 0, ('Reset',0), ('System',0), 0, ('RemainTime',1), ('DurationTime',1),
    0, ('ImageHorizon',1), ('ImageVertical',1), 0, ('Strstr',1), ('String',1), 0, 0,#60
    ('ModuleFileName',1), 0, 0, ('OP_06B',1), ('Time',1), 0, ('OP_06E',1), 0,
    ('Conquest',1), ('Message',1), ('Array',0), ('AssocArray',0), ('Count',1), ('VariableValue',1), 0, 0,#70
    0, 0, ('LockVideo',0), 0, 0, 0, 0, 0,
  )
  def pSys(self):
    if self.opcode<0x80:
      fun_info=self.sysfunc[self.opcode]
    if self.opcode>0x80:
      self.text.append('\t'*self.tabcount+'OP_%03X'%self.opcode+self.makearg(self.argc))
      return
    elif fun_info==0:
      self.stack.append('OP_%03X'%self.opcode+self.makearg(self.argc))
      return
    if fun_info[1]:
      self.stack.append(fun_info[0]+self.makearg(self.argc))
    else:
      self.text.append('\t'*self.tabcount+fun_info[0]+self.makearg(self.argc))
  def p8a(self):
    'unk'
    self.pSys()
  def p8e(self):
    '分号'
    if len(self.stack)==0:
      self.text[-1]+=';'
    elif len(self.stack)==1:
      self.text.append('\t'*self.tabcount+self.stack.pop()+';')
    else:
      pdb.set_trace();
  def p91(self):
    '似乎是引用'
    self.stack[-1]=='@'+self.stack[-1]
  def p94(self):
    'chapter起始标志'
    self.chkargc(1)
    self.text.append(self.readarg())
  def p95(self):
    'scene起始'
    self.chkargc(1)
    self.text.append(self.readarg())
  def p97(self):
    'function起始标志'
    self.text.append(', '.join([self.readarg() for i in range(self.argc)]))
  def p98(self):
    'if判断'
    self.chkargc(1)
    self.readarg()
    self.text.append('\t'*self.tabcount+'if('+self.stack.pop()+')')
  def p99(self):
    'while循环'
    self.chkargc(1)
    self.readarg()
    self.text.append('\t'*self.tabcount+'while('+self.stack.pop()+')')
  def p9a(self):
    'select'
    self.text.append('\t'*self.tabcount+'select')
    self.readarg()
  def p9b(self):
    'case'
    self.text.append('\t'*self.tabcount+'case '+self.readarg())
    self.readarg()
    self.readarg()
  def p9c(self):
    'unk'
    self.pSys()
  def p9d(self):
    'unk'
    self.chkargc(0)
    self.pSys()
  def pa2(self):
    'call chapter'
    self.text.append('\t'*self.tabcount+'call_chapter '+self.readarg())
  def pa3(self):
    'call scene调用'
    self.text.append('\t'*self.tabcount+'call_scene '+self.readarg())
  def pa5(self):
    '加法'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' + '+var1+')')
  def pa6(self):
    '减法'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' - '+var1+')')
  def pa7(self):
    '乘以'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' * '+var1+')')
  def pa8(self):
    '除以'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' / '+var1+')')
  def pa9(self):
    '取模'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' % '+var1+')')
  def paa(self):
    '位于?'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' & '+var1+')')
  def pab(self):
    '位或?'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' | '+var1+')')
  def pac(self):
    '非运算'
    self.stack[-1]='!'+self.stack[-1]
  def pae(self):
    'unk运算'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' ?ae '+var1+')')   
  def pb0(self):
    '赋值运算，将栈中的运算结果赋给指定变量'
    self.chkargc(1)
    inst=self.readarg()
    if inst=='__array_variable__':
      var1=self.stack.pop()
      self.text.append('\t'*self.tabcount+self.stack.pop()+' = '+var1)
    else:
      self.text.append('\t'*self.tabcount+inst+' = '+self.stack.pop())
  def pb1(self):
    'unk?应该和赋值有关'
    self.chkargc(1)
    self.text.append('\t'*self.tabcount+self.readarg()+' += '+self.stack.pop())
  def pb2(self):
    'unk?赋值？'
    self.chkargc(1)
    self.text.append('\t'*self.tabcount+self.readarg()+' -= '+self.stack.pop())
  def pb8(self):
    'unk?'
    self.chkargc(1)
    self.text.append('\t'*self.tabcount+self.readarg()+' ?b8 '+self.stack.pop())    
  def pb9(self):
    'unk?'
    self.chkargc(1)
    inst=self.readarg()
    if inst=='__array_variable__':
      var1=self.stack.pop()
      self.text.append('\t'*self.tabcount+self.stack.pop()+' ?b9 '+var1)
    else:
      self.text.append('\t'*self.tabcount+inst+' ?= '+self.stack.pop())    
  def pbe(self):
    '步增'
    self.text.append('\t'*self.tabcount+self.stack.pop()+'++')
  def pbf(self):
    '步减'
    self.text.append('\t'*self.tabcount+self.stack.pop()+'--')
  def pc0(self):
    '等于判断'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' == '+var1+')')
  def pc1(self):
    '栈中元素非等判断'
    var1=self.stack.pop()
    var2=self.stack.pop()
    self.stack.append('('+var2+' != '+var1+')')
  def pc2(self):
    '<='
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' <= '+var1+')')
  def pc3(self):
    '>='
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' >= '+var1+')')
  def pc4(self):
    '<'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' < '+var1+')')
  def pc5(self):
    '>'
    var1=self.stack.pop()
    self.stack.append('('+self.stack.pop()+' > '+var1+')')
  def pc6(self):
    '条件与'
    var1=self.stack.pop()
    var2=self.stack.pop()
    self.stack.append('('+var2+' && '+var1+')')
  def pc7(self):
    '条件或'
    var1=self.stack.pop()
    var2=self.stack.pop()
    self.stack.append('('+var2+' || '+var1+')')
  def pc8(self):
    "左大括号"
    self.text.append('\t'*self.tabcount+'{')
    self.tabcount+=1
  def pc9(self):
    '右大括号'
    if len(self.stack)==1:
      self.text.append('\t'*self.tabcount+self.stack.pop())
    elif len(self.stack)>1:
      pdb.set_trace()
    self.tabcount-=1
    self.text.append('\t'*self.tabcount+'}')
  def pce(self):
    '调用函数'
    if self.argc==0: pdb.set_trace()
    name=self.readarg()
    self.stack.append(name+self.makearg(self.argc-1))
##    self.text.append('\t'*self.tabcount+name+self.makearg(self.argc-1))
  def pcf(self):
    'else'
    self.chkargc(1)
    self.readarg()
    self.text.append('\t'*self.tabcount+'else')
  def pd0(self):
    '常量入栈'
    self.chkargc(2)
    t0=self.readarg()
    if t0=='INT':
      self.stack.append(self.readarg())
    elif t0=='STRING':
      self.stack.append('"'+self.readarg()+'"')
    elif t0=='FLOAT':
      self.stack.append(self.readarg())
    else:
      pdb.set_trace()
  def pd1(self):
    '变量入栈'
    self.stack.append(self.readarg())
  def pd2(self):
    '添加负号'
    self.stack[-1]=='-'+self.stack[-1]
  def pd4(self):
    'chapter结束标志'
    self.text.append('')
    self.readarg()
  def pd5(self):
    'scene结束'
    self.text.append('')
    self.readarg()
  def pd7(self):
    'function结束'
    self.text.append('')
    self.text.append('OP_D7')
  def pd8(self):
    '显示脚本'
    self.chkargc(3)
    self.readarg()
    self.readarg()
    self.text.append('')
    self.text.append(self.readarg())
    self.text.append('')
  def pd9(self):
    self.text.append('\t'*self.tabcount+'OP_0D9')
  def pdd(self):
    'case结束'
    self.text.append('')
    self.readarg()
  def pde(self):
    '数组索引运算'
    self.chkargc(2)
    name=self.readarg()
    count=int(self.readarg())
    if count==0:
      pdb.set_trace()
    self.stack.append(name+'['+']['.join(self.stack[-count:])+']')
    del self.stack[-count-1:-1]
  def pdf(self):
    'unk'
    self.text.append('\t'*self.tabcount+'OP_DF')
  def pe0(self):
    'unk'
    self.text.append('\t'*self.tabcount+'OP_E0')
  def pe4(self):
    'unk'
    self.text.append('\t'*self.tabcount+'OP_E4')
  def pf7(self):
    '似乎什么都没做'
    self.text.append('\t'*self.tabcount+'OP_F7_NOP')
  def pf8(self):
    'unk'
    self.text.append('\t'*self.tabcount+'OP_F8'+self.makearg(self.argc))
  def pff(self):
    'unk'
    self.pSys()
  def p100(self):
    'unk'
    self.pSys()
  def p101(self):
    'unk'
    self.text.append('\t'*self.tabcount+'OP_101')
  def p104(self):
    'unk'
    self.pSys()
  def p105(self):
    'unk'
    self.pSys()
  def p10e(self):
    'unk字体相关'
    self.pSys()
  def p10f(self):
    self.stack.append('OP_10F'+self.makearg(self.argc))
  def p110(self):
    self.stack.append('OP_110'+self.makearg(self.argc))
  def p116(self):
    self.stack.append('OP_116'+self.makearg(self.argc))
  def p11c(self):
    'unk'
    self.text.append('\t'*self.tabcount+'OP_11C'+self.makearg(self.argc))
  
  optable=(
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, p8a, 0, 0, 0, p8e, 0, #8
    0, p91, 0, 0, p94, p95, 0, p97, p98, p99, p9a, p9b, p9c, p9d, 0, 0,
    0, 0, pa2, pa3, 0, pa5, pa6, pa7, pa8, pa9, paa, pab, pac, 0, pae, 0,
    pb0, pb1, pb2, 0, 0, 0, 0, 0, pb8, pb9, 0, 0, 0, 0, pbe, pbf, #B
    pc0, pc1, pc2, pc3, pc4, pc5, pc6, pc7, pc8, pc9, 0, 0, 0, 0, pce, pcf,
    pd0, pd1, pd2, 0, pd4, pd5, 0, pd7, pd8, pd9, 0, 0, 0, pdd, pde, pdf,
    pe0, 0, 0, 0, pe4, 0, 0, 0, pSys, 0, pSys, 0, 0, 0, 0, 0, #E
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, p10f, #10
    p110, 0, 0, 0, 0, 0, p116, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  )
  optable1=[
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, #8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, #B
    0, 0, 0, 0, 0, 0, 0, 0, pc8, pc9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, #E
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, #11
  ]
  def Parse(self):
    while(self.nsb.tell()<len(self.nsb)-1):
      sno, self.opcode, self.argc=unpack('IHH',self.nsb.read(8))
      if self.opcode<0x80:
        self.pSys()
      elif self.optable[self.opcode-0x80]!=0:
        self.optable[self.opcode-0x80](self)
      elif self.opcode>0xe0:
        self.pSys()
      else:
        print 'code: %x'%self.opcode
        pdb.set_trace()
    return '\n'.join(self.text)

  def SimpleParse(self):
    while(self.nsb.tell()<len(self.nsb)-1):
      sno, self.opcode, self.argc=unpack('IHH',self.nsb.read(8))
      if self.optable1[self.opcode]!=0:
        self.optable1[self.opcode](self)
      else:
        inst='%02X '%self.opcode
        for i in range(self.argc):
          lens=self.nsb.readu32()
          inst+=self.nsb.read(lens)
          if i!=self.argc-1:
            inst+=','
        self.text.append('\t'*self.tabcount+inst)
    return '\n'.join(self.text)
