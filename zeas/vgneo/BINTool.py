WindowsBatchString='''
@echo off
E:\tools\python-2.5.2\python.exe %~0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto :END
'''

import os
import sys
import pdb
import struct

BINAna='''
//U32[2] 0, U32
//U16 Op
//U16[] Args
//Op	Name	Arg[n]

6		JP06	OFFSET
7		JP07	OFFSET
B		STR		STR
'''

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

def SplitTxt(Src):
	Src=Src.replace(', ',' ')
	if Src.count('"')==2:
		return Src[:Src.find('"')].split()+[Src[Src.find('"')+1:Src.rfind('"')]]+Src[Src.rfind('"')+1:].split()
	return Src.split()

def SplitANA(Data):
	if '//' in Data:
		Data=Data[:Data.find('//')]
	Data=Data.split()
	n=0
	while n<len(Data):
		if '[' in Data[n]:
			pQuote=Data[n].find('[')
			Data=Data[:n]+[Data[n][:pQuote]]*int(Data[n][pQuote+1:Data[n].find(']')])+Data[n+1:]
		n+=1
	return Data

def CalcStrLen(Src):
	pSrc=0
	while Src[pSrc]!='\0': pSrc+=1
	return pSrc

def AnaBin(BINFile):
	ANAFile=MyStr(BINAna)
	OpCode={}
	Src=ANAFile.readline()
	while Src:
		if Src[:2]=='//' or Src[0]=='\n':
			Src=ANAFile.readline()
			continue
		Src=SplitANA(Src)
		OpCode[int(Src[0],16)]=Src[1:]
		Src=ANAFile.readline()
	BINFile=open(sys.argv[1],'rb')
	Dummy1, Dummy2, nCode, CodeSize=struct.unpack('iiii', BINFile.read(16))
	if Dummy1!=Dummy2!=0:
		print "Reserved Data Error!"
		os.system('pause')
		sys.exit()
	CodeBlockTab=[]
	for n in range(nCode):
		CodeBlockTab.append(struct.unpack('i', BINFile.read(4))[0])
	BINFile=MyStr(BINFile.read())
	Count=1
	JmpTable=[]
	CodeTable=[]
	OffTable=[]
	CurOff=BINFile.tell()
	Res='#GIGA_BIN_Resource\n\n'
	nRes=0
	while CurOff<CodeSize-5:
		Op=struct.unpack('H',BINFile.read(2))[0]
		if OpCode.has_key(Op):
			TXTStr=OpCode[Op][0]+' '
			for Type in OpCode[Op][1:]:
				if Type=='DWORD':
					TXTStr+=str(struct.unpack('i',BINFile.read(4))[0])+', '
				elif Type=='WORD':
					TXTStr+=str(struct.unpack('h',BINFile.read(2))[0])+', '
				elif Type=='BYTE':
					TXTStr+=str(ord(BINFile.read(1)))+', '
				elif Type=='STR' or Type=='TEXT':
					StrLen=struct.unpack('i',BINFile.read(4))[0]
					TXTStr+='<%04d>, '%nRes
					Res+='<%04d> '%nRes+BINFile.read(StrLen).strip('\0').replace('\r','\\r').replace('\n','\\n')+'\n'
					nRes+=1
				elif Type=='OFFSET':
					JmpTable.append([Count, struct.unpack('I',BINFile.read(4))[0]])
					TXTStr+='OfFsEt, '
				else:
					print "Unknown Type or Wrong Assert."
					sys.exit()
			CodeTable.append('%d\t'%(Count*10)+TXTStr.strip(', '))
		else:
			TXTStr='OP'+hex(Op).upper()[2:]+' '+str(struct.unpack('i',BINFile.read(4))[0])
			CodeTable.append('%d\t'%(Count*10)+TXTStr)
		OffTable.append(CurOff)
		Count+=1
		CurOff=BINFile.tell()
	for Jmp in JmpTable:
		for n in range(len(OffTable)):
			if Jmp[1]<=OffTable[n]: break
		if Jmp[1]!=OffTable[n]:
			print "Error Offset %X" % Jmp[1]
			#CodeTable=CodeTable[:Jmp[0]-1]+[CodeTable[Jmp[0]-1].replace('OfFsEt', '%d+%d'%(n*10+10, OffTable[n]-Jmp[1]))]+CodeTable[Jmp[0]:]
			sys.exit()
		CodeTable=CodeTable[:Jmp[0]-1]+[CodeTable[Jmp[0]-1].replace('OfFsEt', '%d'%(n*10+10))]+CodeTable[Jmp[0]:]
	CodeFile=open(sys.argv[1]+'.code','w')
	CodeFile.write('#CODEBLOCK\n')
	for Jmp in CodeBlockTab:
		if Jmp==-1:
			CodeFile.write('EOF\n')
			continue
		for n in range(len(OffTable)):
			if Jmp<=OffTable[n]: break
		if OffTable and Jmp!=OffTable[n]:
			print "Error Offset %X" % Jmp
			os.system('pause')
			sys.exit()
		CodeFile.write(str(n*10+10)+'\n')
	CodeFile.write('\n#CODE\n'+'\n'.join([Code for Code in CodeTable]))
	open(sys.argv[1]+'.txt','w').write(Res)
	print "Finished."

def PackBIN(CodeFile, TXTFile, OUTFile):
	ANAFile=MyStr(SAna)
	OpCode={}
	Src=ANAFile.readline()
	while Src:
		if Src[:2]=='//' or Src[0]=='\n':
			Src=ANAFile.readline()
			continue
		Src=SplitANA(Src)
		OpCode[Src[1]]=Src[:1]+Src[2:]
		Src=ANAFile.readline()
	TXTFile=open(TXTFile,'r')
	if TXTFile.readline()[0]=='<': TXTFile.seek(0)
	else: TXTFile.readline()
	Res={}
	Src=TXTFile.readline()
	while Src:
		if Src[0]=='<':
			Res[Src[1:Src.find('>')]]=Src[Src.find('> ')+2:-1].replace('\\n', '\n').replace('\\r'. '\r')+'\0'
			Src=TXTFile.readline()
			continue
		Src=TXTFile.readline()
	TXTFile.close()
	CodeFile=open(CodeFile,'r')
	if TXTFile.readline().strip()!="#CODEBLOCK":
		print "Invalid Code Format!"
		sys.exit()
	CodeBlockTab=[]
	Src=CodeFile.readline().strip()
	while Src:
		if Src=='EOF':
			CodeBlockTab.append(-1)
		else:
			CodeBlockTab.append(int(Src))
		Src=CodeFile.readline().strip()
	JmpTable=[]
	CodeTable={}
	OffTable=[]
	CurOff=0
	CodeStr=''
	Count=0
	Src=CodeFile.readline()
	while Src:
		if Src[0]=='\n' or Src[0]=='#':
			Src=TXTFile.readline()
			continue
		Src=Src.split()
		if Count<int(Src[0]): Count=int(Src[0])
		else:
			print "Exception: Uncontinual SieralNumber @ line %d" % Line
			sys.exit()
		CodeTable[Count]=len(CodeStr)
		if OpCode.has_key(Src[1]):
			Args=''
			n=2
			for Type in OpCode[Src[1]][1:]:
				if Type=='DWORD':	Args+=struct.pack('i',int(Src[n]))
				elif Type=='WORD':	Args+=struct.pack('h',int(Src[n]))
				elif Type=='BYTE':	Args+=chr(int(Src[n]))
				elif Type=='STR':
					Sentance=Res[Src[n].strip('<>')]
					Args+=struct.pack('I',len(Sentance))+Sentance
				elif Type=='OFFSET':
					if int(Src[n])<Count:
						Args+=struct.pack('I', CodeTable[int(Src[n])])
					else:
						JmpTable.append([int(Src[n]), len(CodeStr)+len(Args)+2])
						Args+='OFST'
				else:
					print "Unknown Type."
					sys.exit()
				n+=1
			CodeStr+=struct.pack('H',int(OpCode[Src[1]][0]))+Args
		elif Src[1][:2]=='OP':
			CodeStr+=struct.pack('H',int(Src[1][2:],16))+struct.pack('i',int(Src[2]))
		else:
			print "Unknown Command %s" % Src[1]
			sys.exit()
		Src=TXTFile.readline()
	Hdr=struct.pack('IIII', 0, 0, len(CodeBlockTab), len(CodeStr))
	for Jmp in CodeBlockTab:
		Hdr+=struct.pack('I', CodeTable[Jmp])
	for Jmp in JmpTable:
		pOff=Jmp[1]
		CodeStr=CodeStr[:pOff]+struct.pack('I', CodeTable[Jmp[0]])+CodeStr[pOff+4:]
	open(OUTFile,'wb').write(Hdr+CodeStr)
	print "Finished."

def Usage():
	print "GIGA BIN Tools\nUsage: %s <BIN>\n\t%s <CODE> <RESTXT> [OUTBIN]"% (sys.argv[0],sys.argv[0])
	sys.exit()
if len(sys.argv)<2: Usage()
elif sys.argv[1].lower()[-4:]=='.bin': AnaBin(sys.argv[1])
elif sys.argv[1].lower()[-5:]=='.code':
	if len(sys.argv)<3: Usage()
	elif len(sys.argv)==3: PutStr(sys.argv[1], sys.argv[2], sys.argv[2]+'.out.bin')
	else: PutStr(sys.argv[1], sys.argv[2], sys.argv[3])
else: Usage()

WindowsBatchString='''
:END
'''