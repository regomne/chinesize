#Python 2.5.2

import os
import sys
import struct

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
	def seek(self, Offset):
		if Offset<len(self):
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

def GetYKSText():
	YKS=open(sys.argv[1],'rb')
	if YKS.read(6)!='YKS001':
		print "Invalid YKS or unsupported version."
		sys.exit()
	dummy=YKS.read(2)
	HdrSize, Reserved, Prg, nPrg, Cmd, nCmd, Res, nRes, Unknown1, Reserved=struct.unpack('I'*10,YKS.read(4*10))
	YKS.seek(Prg)
	Prg=[struct.unpack('I',YKS.read(4))[0] for n in range(nPrg)]
	Cmd=[struct.unpack('Iiii',YKS.read(16)) for n in range(nCmd)]
	Res=YKS.read(nRes)
	YKS=open(sys.argv[1]+'.txt','w')
	YKS.write('YKS 1.00 %d\n\n' % Unknown1)
	pPrg=0
	# Op:	(Arg1,	Arg2,	Arg3,	nNext,	Name)
	S="STR"
	D="DWORD"
	W='WORD'
	B="BYTE"
	F="FLAG"
	V="VALUE"
	Dic={0: (S,		V,		F,		1,		'COL'),\
		1:	(S,		D,		-1,		0,		'LBL'),\
		4:	(0,		D,		0,		0,		'U32'),\
		5:	(0,		S,		0,		0,		'STR'),\
		8:	(S,		0,		D,		0,		'FLG'),\
		9:	(S,		0,		D,		0,		'OP9'),\
		10:	(0,		V,		0,		0,		'OPA')}
	TxtStr=''
	while pPrg<len(Prg):
		Op=Cmd[Prg[pPrg]]
		if not Dic.has_key(Op[0]):
			print "Unknown Op %X" % Op[0]
		TxtStr+=Dic[Op[0]][4]+' '
		for n in range(3):
			if Dic[Op[0]][n]==S:
				TxtStr+='"'+Res[Op[n+1]:Res.find('\0',Op[n+1])]+'", '
			elif Dic[Op[0]][n]==D:
				TxtStr+=str(struct.unpack('i', Res[Op[n+1]:Op[n+1]+4])[0])+', '
			elif Dic[Op[0]][n]==F:
				if Op[n+1]==1: TxtStr+='True, '
				elif Op[n+1]==0: TxtStr+='False, '
				else:
					print "Error BOOL type!"
					sys.exit()
			elif Dic[Op[0]][n]==V:
				TxtStr+=str(Op[n+1])+', '
			elif Dic[Op[0]][n]!=Op[n+1]:
				print "Op %X may contain Unknown Type %s!" % (Op[0], Dic[Op[0]][n])
				sys.exit()
			else:
				pass
		for n in range(Dic[Op[0]][3]):
			TxtStr+=str(Prg[pPrg+1])+', '
			pPrg+=1
		TxtStr=TxtStr[:-2]+'\n'
		pPrg+=1
	YKS.write(TxtStr)
	TxtStr=MyStr(TxtStr)
	TxtStr.readline()
	TxtStr.readline()
	Src=TxtStr.readline()
	CStr=''
	PreCol=0
	while Src:
		if Src=='\n':
			Src=TxtStr.readline()
			continue
		Src=SplitTxt(Src)
		if Src[0]=='COL':
			if PreCol: CStr=CStr[:-1]+'\n'
			CStr+=Src[1].strip('"')+' '
			PreCol=1
		elif Src[0]=='STR':	CStr+=Src[1].replace('\\','\\\\')+' '
		elif Src[0]=='LBL':
			if PreCol: CStr=CStr[:-1]+'\n'
			CStr+='*'+Src[1].strip('"')+'\n'
			PreCol=0
		elif Src[0]=='U32':	CStr+=Src[1]+' '
		elif Src[0]=='FLG':
			if PreCol: CStr=CStr[:-1]+'\n'
			CStr+=Src[1].strip('"')+' '+Src[2]+' '
			PreCol=0
		Src=TxtStr.readline()
	open(sys.argv[1]+'.c','w').write(CStr)
	CStr=MyStr(CStr)
	Filename=os.path.split(sys.argv[1])[1][:-4]
	KStr='*YKS%s\n@txt filename=%s.txt\n'%(Filename, Filename)
	Count=1
	Src=CStr.readline()
	beforeText=0
	while Src:
		if Src[0]=='*' and not(122<ord(Src[1])<128 or ord(Src[1])<65):
			KStr+=Src.split()[0]+'\n@txt filename=%s.txt\n' % Filename
			Src=CStr.readline()
			continue
		if 122<ord(Src[0])<128 or ord(Src[0])<65:
			Src=CStr.readline()
			continue
		Src=Src.split()
		if Src[0]=='if' or Src[0]=='else':
			Src=CStr.readline()
			continue
		elif Src[0]=='StrOut':
			KStr+=Src[1].strip('"')+'\n'
			beforeText=1
			Src=CStr.readline()
			continue
		elif Src[0]=='PF' and beforeText:
			KStr+='@tr count=%d\n@p' % Count
			Count+=1
			beforeText=0
			Src=CStr.readline()
			continue
		elif Src[0]=='KeyWait' and beforeText:
			KStr+='@tr count=%d\n@l\n' % Count
			Count+=1
			beforeText=0
			Src=CStr.readline()
			continue
		else: KStr+='@'+Src[0]+' '
		for n in range(1, len(Src)):
			KStr+='arg%d=%s '%(n, Src[n])
		KStr=KStr[:-1]+'\n'
		Src=CStr.readline()
	open(sys.argv[1]+'.ks','w').write(KStr)

def SplitTxt(Src):
	Src=Src.replace(', ',' ')
	if Src.count('"')==2:
		return Src[:Src.find('"')].split()+[Src[Src.find('"'):Src.rfind('"')+1]]+Src[Src.rfind('"')+1:].split()
	return Src.split()

def ImportYKS():
	# Name:	(Arg1,	Arg2,	Arg3,	nNext,	Op)
	S="STR"
	D="DWORD"
	W='WORD'
	B="BYTE"
	F="FLAG"
	V="VALUE"
	Dic={'COL': (S,		V,		F,		1,		0),\
		'LBL':	(S,		D,		-1,		0,		1),\
		'U32':	(0,		D,		0,		0,		4),\
		'STR':	(0,		S,		0,		0,		5),\
		'FLG':	(S,		0,		D,		0,		8),\
		'OP9':	(S,		0,		D,		0,		9),\
		'OPA':	(0,		V,		0,		0,		10)}
	TXT=open(sys.argv[1],'r')
	TxtStr=TXT.readline().split()
	Unknown=int(TxtStr[2])
	TxtStr=TXT.readline()
	Res=''
	Cmd=[]
	Prg=''
	while TxtStr:
		if TxtStr=='\n':
			TxtStr=TXT.readline()
			continue
		TxtStr=SplitTxt(TxtStr)
		if not Dic.has_key(TxtStr[0]):
			print "Unknown Op name: %s" % TxtStr[0]
			sys.exit()
		CurCmd=struct.pack('I', Dic[TxtStr[0]][4])
		pTxt=1
		for n in range(3):
			if Dic[TxtStr[0]][n]==S:
				CurStr=TxtStr[pTxt].strip('"')+'\0'
				pTxt+=1
			elif Dic[TxtStr[0]][n]==D:
				CurStr=struct.pack('i', int(TxtStr[pTxt]))
				pTxt+=1
			elif Dic[TxtStr[0]][n]==V:
				CurCmd+=struct.pack('i', int(TxtStr[pTxt]))
				pTxt+=1
			elif Dic[TxtStr[0]][n]==F:
				if TxtStr[pTxt]=='True': CurCmd+='\1\0\0\0'
				elif TxtStr[pTxt]=='False': CurCmd+='\0\0\0\0'
				else:
					print "Unexpected BOOL Type %s" % TxtStr[pTxt]
					sys.exit()
				pTxt+=1
			else:
				CurCmd+=struct.pack('i', Dic[TxtStr[0]][n])
			if Dic[TxtStr[0]][n]==S or Dic[TxtStr[0]][n]==D:
				if CurStr in Res:
					CurCmd+=struct.pack('I', Res.find(CurStr))
				else:
					CurCmd+=struct.pack('I', len(Res))
					Res+=CurStr
		if CurCmd in Cmd:
			Prg+=struct.pack('I', Cmd.index(CurCmd))
		else:
			Prg+=struct.pack('I', len(Cmd))
			Cmd.append(CurCmd)
		for n in range(-Dic[TxtStr[0]][3], 0):
			Prg+=struct.pack("i",int(TxtStr[n]))
		TxtStr=TXT.readline()
	Cmd=''.join(Cmd)
	CurOff=0x30
	HdrStr='YKS001\0\0'+struct.pack('I'*10, \
			0x30, 0, 0x30, len(Prg)/4,\
			0x30+len(Prg), len(Cmd)/16, 0x30+len(Prg)+len(Cmd), len(Res),\
			Unknown, 0)
	open(sys.argv[1]+'.yks','wb').write(HdrStr+Prg+Cmd+Res)

def Trans():
	TXT=open(sys.argv[1])
	CFile=open(sys.argv[1]+'.c')
	TXT.readline()
	TxtStr=SplitTxt(TXT.readline())
	while TxtStr:
		if TxtStr[0]=='OPA':
			CFile.write('Value'+TxtStr[1]+' ')
		elif TxtStr[0]=='COL':
			CFile.write(TxtStr[1]+' ')

if len(sys.argv)>1:
	if sys.argv[1][-4:].lower()=='.txt': ImportYKS()
	elif sys.argv[1][-4:].lower()=='.yks': GetYKSText()
	print "Finished."
	sys.exit()
print "YKS 1.00 Tool\nUsage: %s <*.yks|*.txt>"% sys.argv[0]
