#py2.7
import os
import sys
import struct
import zlib
from ctypes import *
import pdb

def Usage():
	print '''Usage: %s <DIR> [Compress]
Compress: 0 - No compress (default)
          3 - zlib
          4 - Reserved for UCI
          Other value will as same as 0'''
	sys.exit()

##if len(sys.argv)<2:
##	Usage()

def Char2Bin(Char):
	Data=[]
	c=ord(Char)
	for n in range(8):
		Data.append(c&1)
		c>>=1
	Data.reverse()
	return Data

def CreateTreeData(CodeList, Code, Data, Tree):
	if type(Tree)==type(''):
		Data+=[0]+Char2Bin(Tree)
		CodeList.append((Tree, Code))
	else:
		Data.append(1)
		CreateTreeData(CodeList, Code+[0], Data, Tree[0])
		CreateTreeData(CodeList, Code+[1], Data, Tree[1])

def Bin2Str(Src):
	Data=[]
	n=0
	for n in range(len(Src)>>3):
		c=0
		for i in range(8):
			c<<=1
			c|=Src[n*8+i]
		Data.append(chr(c))
	Remain=Src[-(len(Src)&7):]
	if len(Src)&7:
		c=0
		for n in range(len(Remain)):
			c|=Remain[n]<<(7-n)
		Data.append(chr(c))
	return ''.join(Data)

def HuffmanCompress(Src):
	TreeWeight=[0]*256
	Src=[ord(c) for c in Src]
	for c in Src:
		TreeWeight[c]+=1
	Tree=[]
	for n in range(256):
		if TreeWeight[n]:
			Tree.append((TreeWeight[n],chr(n)))
	Tree.sort(reverse=1)
	while len(Tree)>1:
		lchild=Tree.pop()
		rchild=Tree.pop()
		Tree.append((lchild[0]+rchild[0],(lchild[1], rchild[1])))
		Tree.sort(reverse=1)
	Tree=Tree[0][1]
	Data=[]
	CharList=[]
	CreateTreeData(CharList, [], Data, Tree)
	CharList.sort()
	ChrList=''.join([n[0] for n in CharList])
	CodeList=[CharList[ChrList.find(chr(n))][1] for n in range(256)]
	for c in Src:
		Data+=CodeList[c]
	return Bin2Str(Data)

def PackPAC(SrcDir, PakFile):
	if not os.path.isdir(SrcDir):	Usage()
	try:
		Compress=int(sys.argv[2])
	except:
		Compress=3
	if Compress not in [3, 4]: Compress=0
	FileList=[]
	for n in os.listdir(SrcDir):
		if os.path.isfile(SrcDir+'\\'+n):
			if os.path.splitext(n)[1].lower() in ['.uci', '.uca', '.dif']: Compress=4
			elif os.path.splitext(n)[1].lower()=='.ifo': continue
			FileList.append(n)
	PakFile=open(PakFile,'wb')
	PakFile.write('PAC\0'+struct.pack('II', len(FileList), Compress))
#	HdrSize=12+76*len(FileList)
	FileInfo={}
	HashList={}
	InfoStr=[]
	FileOffset=12
	DiffList=[]
	print "Packing..."
	for n in FileList:
		FileData=open(SrcDir+'\\'+n,'rb').read()
		hash=FileData.__hash__()
		Uncmpr=len(FileData)
		Cmpr=len(FileData)
		if Compress==3:
			FileData=zlib.compress(FileData,9)
			Cmpr=len(FileData)
		elif Compress==4:
			if os.path.splitext(n)[1].lower()=='.uci':
				bpp=int(FileData[3])
				w, h = struct.unpack('II', FileData[4:12])
				w=(w+3)&0xFFFFFFFC
				Uncmpr=w*h*bpp+54
				n=os.path.splitext(n)[0]+'.BMP'
			elif os.path.splitext(n)[1].lower()=='.dif':
				if FileData[:4]!='ZDIF':
					DiffList.append(n)
					continue
				n=os.path.splitext(n)[0]+'.BMP'
				bpp=int(FileData[31])
				w, h = struct.unpack('II', FileData[32:40])
				w=(w+3)&0xFFFFFFFC
				Uncmpr=w*h*bpp+54
			elif os.path.splitext(n)[1].lower()=='.uca':
				n=os.path.splitext(n)[0]
				Uncmpr=struct.pack('i',-1)
				UCADll.UCADecode2(FileData, Cmpr, 0, Uncmpr, 44100)
				Uncmpr=struct.unpack('I',Uncmpr)[0]
			else:
				FileData=zlib.compress(FileData,9)
				Cmpr=len(FileData)
		FileInfo[os.path.splitext(n)[0].upper()]=(FileOffset, Uncmpr, Cmpr) #for create diff list
		n=n.decode('gbk').encode('shiftjis').replace('_zeas_', '\x81E')
		if hash in HashList:
#			pdb.set_trace()
			InfoStr.append(n+'\0'*(64-len(n))+HashList[hash])
		else:
			Info=struct.pack('III', FileOffset, Uncmpr, Cmpr)
			HashList[hash]=Info
			InfoStr.append(n+'\0'*(64-len(n))+Info)
			PakFile.write(FileData)
			FileOffset+=Cmpr
	for n in DiffList:
		Base, Diff, X, Y, W, H=open(SrcDir+'\\'+os.path.splitext(n)[0]+'.ifo','r').read().strip().split(',')
		if Base.upper() not in FileInfo:
			pdb.set_trace()
		Base=FileInfo[Base.upper()]
		DiffData='ZDIF'+struct.pack('IIII', Base[0], Base[2], int(X), int(Y))+open(SrcDir+'\\'+n,'rb').read()
		n=os.path.splitext(n)[0]+'.BMP'
		Cmpr=len(DiffData)
		hash=DiffData.__hash__()
		n=n.decode('gbk').encode('shiftjis').replace('_zeas_', '\x81E')
		if hash in HashList:
			pdb.set_trace()
			InfoStr.append(n+'\0'*(64-len(n))+HashList[hash])
		else:
			Info=struct.pack('III', FileOffset, Base[1], Cmpr)
			HashList[hash]=Info
			InfoStr.append(n+'\0'*(64-len(n))+Info)
			PakFile.write(DiffData)
			FileOffset+=Cmpr
	InfoStr.sort()
	FileInfo=''.join([chr(ord(c)^0xFF) for c in HuffmanCompress(''.join(InfoStr))])
	PakFile.write(FileInfo)
	PakFile.write(struct.pack('I',len(FileInfo)))
	print "%d files.\nFinished." % len(FileList)
	sys.exit()

#UCADll=WinDLL(r'E:\tools\uca\ucadec.dll')

#PackPAC(sys.argv[1], sys.argv[1]+'.pac')

##PackPAC(r'F:\Program Files\majikoi\NewScript',\
##        r'F:\Program Files\majikoi\Script.pac')

PackPAC(r'D:\chinesize\NeXAs\nscript',\
        r'D:\chinesize\NeXAs\nscript.pac')
