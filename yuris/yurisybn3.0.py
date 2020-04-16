import os
import sys
import struct
import re

class MyStr(str):
	"Operate String as a File."
	pData=0
	def __init__(self, Src):
		str.__init__(self)
		self.pData=0
		self=Src
	def read(self, Bytes=-1):
		if Bytes<-1:
			return ''
		elif Bytes==-1:
			tData=self[self.pData:]
			self.pData=len(self)
		else:
			tData=self[self.pData:self.pData+Bytes]
			self.pData+=Bytes
		return tData
	def tell(self):
		return self.pData
	def seek(self, Offset, mode=0):
		if mode==1: Offset+=self.pData
		elif mode==2: Offset=len(self)-Offset
		if Offset<0:
			self.pData+=Offset
		elif Offset<len(self):
			self.pData=Offset
		else:
			self.pData=len(self)
	def readline(self, nLine=1):
		if nLine<1: nLine=1
		if '\n' in self[self.pData:]:
			tData=self[self.pData:self.find('\n',self.pData)+1]
		else:
			tData=self[self.pData:]
		self.pData+=len(tData)
		return tData

def STR(SRC):
	RES=[]
	while 1:
		c=SRC.read(1)
		if c=='\0': break
		elif c=='\n': c='\\n'
		elif c=='\r': c='\\r'
		elif c=='\\': c='\\\\'
		RES.append(c)
	return ''.join(RES)

def YSER(YBN):
	TXT=open(sys.argv[1]+'.txt','w')
	ver, count, reserved = struct.unpack('III', YBN.read(12))
	TXT.write('#YSER v%.2f\n\n'%(ver/100.))
	for n in range(count):
		TXT.write('%d, '%struct.unpack('I',YBN.read(4)))
		TXT.write(STR(YBN)+'\n')
	TXT.close()

def YSCM(YBN):
	TXT=open(sys.argv[1]+'.txt','w')
	ver, count, reserved = struct.unpack('III', YBN.read(12))
	TXT.write('#YSCM v%.2f\n\n'%(ver/100.))
	for n in range(count):
		TXT.write(STR(YBN)+'\n')
		for n in range(ord(YBN.read(1))):
			TXT.write('\t%s, %d, %d\n'%(STR(YBN), ord(YBN.read(1)), ord(YBN.read(1))))
	TXT.close()

def NULL(YBN):
	print 'not processed.'

def YSTL(YBN):
	TXT=open(sys.argv[1]+'.txt','w')
	ver, count = struct.unpack('II', YBN.read(8))
	TXT.write('#YSTL v%.2f\n\n'%(ver/100.))
	for n in range(count):
		if struct.unpack('I', YBN.read(4))[0]>count: break
		STRLEN=struct.unpack('I', YBN.read(4))[0]
		TXT.write(YBN.read(STRLEN)+'\n')
		dummy=YBN.read(16)
	TXT.close()

def YSLB(YBN):
	TXT=open(sys.argv[1]+'.txt','w')
	ver, count = struct.unpack('II', YBN.read(8))
	TXT.write('#YSLB v%.2f\n\n'%(ver/100.))
	List=[struct.unpack('I', YBN.read(4))[0] for n in range(256)]
	for n in range(count):
		TXT.write(YBN.read(ord(YBN.read(1))).ljust(36)+
			", %08X, %6d, %5d, %3d, %3d\n"%struct.unpack('IIHBB',YBN.read(12)))
	TXT.close()

def YSTB(YBN):
	'''u32 'YSTB'
	u32 ver
	u32 count
	u32 Code_Size=count*4
	u32 Arg_Size=count*12
	u32 Resource_Size
	u32 T4_Size=count*4
	u32 reserved==0
	====================
	Code:
		u8 [0x1b, 0x1c]
		u8 nArg(sizeof()=0xC)
		u16 Reserved
	Arg:u8
		u8
		u16
		u32 ResLength
		u32 ResOffset(of Resource_section)
	Resource:
		struct{
			u8 Type
			u16 ResLen
			char[] ResStr
			}[]
	JmpList: u32[]
	'''
	TXT=open(sys.argv[1]+'.code','w')
	ver, count, Code_Size, Arg_Size, Res_Size, OffList_Size, reserved = \
		struct.unpack('IIIIIII', YBN.read(28))
	TXT.write('#YSTB v%.2f\n\n'%(ver/100.))
	Code=MyStr(YBN.read(Code_Size))
	Args=MyStr(YBN.read(Arg_Size))
	Res=MyStr(YBN.read(Res_Size))
	OffList=struct.unpack('I'*count, YBN.read(OffList_Size))
	Text=[]
	Count=0
	CodeStr=[]
	for n in range(count):
		OP, nArg, Reserved=struct.unpack('BBH',Code.read(4))
		CodeStr.append('OP%02X'%OP)
		if Reserved:
			print 'Warning: Reserved value is used !'
			os.system('pause')
		for n in range(nArg):
			Arg, ArgType, ResLen, ResOff = struct.unpack('HHII',Args.read(12))
			Res.seek(ResOff)
			ResStr=[]
			if ResOff+ResLen>len(Res): ResLen=len(Res)-ResOff
			IsText=0
			while Res.tell()<ResOff+ResLen:
				Type=ord(Res.read(1))
				if Type>0x7f:
					Res.seek(-1,1)
					Count+=1
					Text.append(Res.read(ResLen))
					ResStr=['<%04d>'%Count]
				else:
					StrLen=struct.unpack('H',Res.read(2))[0]
					#if StrLen>ResLen: pdb.set_trace()
					Resource=Res.read(StrLen)
					if not Type==0x4d:
						Resource=''.join(['%02X'%ord(n) for n in Resource])
					elif Resource[0]==Resource[-1]=='"':
						for n in Resource:
							if ord(n)>0x7f:
								IsText=1
								break
					if IsText:
						Count+=1
						Text.append(Resource[1:-1])
						Resource='"<%04d>"'%Count
						IsText=0
					ResStr.append('%X:'%Type+Resource)
			CodeStr.append('\t%X:%X\t%s'%(Arg, ArgType, ', '.join(ResStr)))
	TXT.write('\n'.join(CodeStr))
	TXT.write('\n\n#Program\n'+', '.join(['%d'%n for n in OffList]))
	if Text: open(sys.argv[1]+'.txt', 'w').write('\n'.join(Text))
	TXT.close()

YSFUNC={
'YSER': YSER,
'YSCM': YSCM,
'YSTD': NULL,
'YSTL': YSTL,
'YSVR': NULL,
'YSLB': YSLB,
'YSTB': YSTB,
}

def CODE2YSTB():
	CODE=open(sys.argv[1],'r')
	TXT=os.path.splitext(sys.argv[1])[0]+'.txt'
	if os.path.isfile(TXT): TXT=[0]+open(TXT, 'r').read().split('\n')
	Src=CODE.readline()
	if Src.strip().lower().split()[0]!='#ystb':
		print 'Wrong code file.'
		sys.exit()
	ver=int(float(Src.strip().split()[1].lower().strip('v'))*100)
	Src=CODE.readline()
	CodeData=[]
	ArgData=[]
	ResData=''
	OffList=[]
	OP=[]
	while Src:
		if Src[0] in [';', '\n']:
			Src=CODE.readline()
			continue
		elif Src[:2]=='OP':
			if OP: CodeData.append(''.join([chr(n) for n in OP]))
			OP=[int(Src[2:],16), 0, 0, 0]
		elif Src[0]=='\t':
			OP[1]+=1
			Res=''
			Arg, Res = [n.strip() for n in Src[1:].strip('\n').split('\t')]
			Arg, ArgType = [int(n,16) for n in Arg.split(':')]
			if not Res: pass
			elif Res[0]=='<':
				Res=TXT[int(Res[1:5])]
			else:
				LQuote=0
				while True:
					LQuote=Res.find('<',LQuote)
					doubleQueta = Res.find('"', LQuote)
					RQuote=Res.find('>',LQuote)
					if LQuote==-1: break
					if (doubleQueta==-1 or RQuote<doubleQueta) and RQuote!=-1:
						Res=Res[:LQuote]+TXT[int(Res[LQuote+1:RQuote])]+Res[RQuote+1:]
					LQuote+=1
				ResLen=0
				ResStr=[]
				for Res1 in re.findall("(?:[^ ]*?:)(?:\"(?: |[^\"])*?\"|.*?)(?:,|$)", Res):
					if Res1[-1]==',':
						Res1 = Res1[0:-1]
					ResType, Str = [n.strip() for n in re.split(':', Res1, 1)]
					if Str and Str[0] in '0123456789ABCDEF':
						HIGH=Str[::2]
						LOW=Str[1::2]
						Str=''.join([chr((int(HIGH[n],16)<<4)|int(LOW[n],16))
							for n in range(len(HIGH))])
					ResStr.append(chr(int(ResType,16))+struct.pack('H', len(Str))+Str)
				Res=''.join(ResStr)
			if Res in ResData:
				ArgData.append(struct.pack('HHII',
					Arg, ArgType, len(Res), ResData.index(Res)))
			else:
				ArgData.append(struct.pack('HHII',
					Arg, ArgType, len(Res), len(ResData)))
				ResData+=Res
		elif Src.strip()=='#Program':
			if OP: CodeData.append(''.join([chr(n) for n in OP]))
			OP=[]
			OffList=''.join([struct.pack('I',int(n)) for n in CODE.readline().split(',')])
		Src=CODE.readline()
	Hdr='YSTB'+struct.pack('II', ver, len(CodeData))
	CodeData=''.join(CodeData)
	ArgData=''.join(ArgData)
	ResData=''.join(ResData)
	CodeData=''.join([chr(ord(CodeData[n])^key[n&3]) for n in xrange(len(CodeData))])
	ArgData=''.join([chr(ord(ArgData[n])^key[n&3]) for n in xrange(len(ArgData))])
	ResData=''.join([chr(ord(ResData[n])^key[n&3]) for n in xrange(len(ResData))])
	OffList=''.join([chr(ord(OffList[n])^key[n&3]) for n in xrange(len(OffList))])
	Hdr+=struct.pack('IIIII',
		len(CodeData), len(ArgData), len(ResData), len(OffList), 0)
	open(sys.argv[1]+'.ybn','wb').write(Hdr+CodeData+ArgData+ResData+OffList)

def ANAYBN():
	YBN=open(sys.argv[1],'rb')
	MAGIC=YBN.read(4)
	if MAGIC in YSFUNC: YSFUNC[MAGIC](YBN)
	else:
		print 'Unknown MAGIC: %s' % MAGIC

key=[0xd3, 0x6f, 0xac, 0x96]
if len(sys.argv)<2:
	print 'YU-RIS YBN v3.00 Script Analyzer/Creator 0.1\nUsage: %s [option] <*.ybn|*.code>\n\nOption:\n    -key <32bit-HEX> specify key to replace the default "D36FAC96".\n         E.g. -key 00000000 means do not crypt the created YBN.' % os.path.split(sys.argv[0])[1]
	sys.exit()
if sys.argv[1]=='-key':
	KeyHi, KeyLo = sys.argv[2][::2], sys.argv[2][1::2]
	key=[(int(KeyHi[n],16)<<4)|int(KeyLo[n],16) for n in range(4)]
	sys.argv[1]=sys.argv[3]
Ext=os.path.splitext(sys.argv[1])[1].lower()
if Ext=='.code':
	CODE2YSTB()
elif Ext=='.ybn':
	ANAYBN()
else:
	sys.exit()