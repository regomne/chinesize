@echo off
rem = '''
set pypath="%~dp0python\python.exe"
if not exist %pypath% set pypath="D:\tools\python-2.6.5\python.exe"
if not exist %pypath% ( echo I can't find "%~dp0python\python.exe" !
pause
goto :EOF
)
%pypath% -x "%~f0" %*
goto :EOF
rem '''

import sys
import os
import struct

def Usage():
	print "Giga VGNeo .PCK Packer\nUsage:\n%s <DirName> [.PacFile]" % os.path.split(sys.argv[0])[1]
	sys.exit()

def PACPackage(SrcDir, PakFile):
	if not os.path.isdir(SrcDir):	Usage()
	FileList=os.listdir(SrcDir)
	FileList.sort()
	Header='PACw'+struct.pack('II',len(FileList),0)
	HdrSize=12+76*len(FileList)
	FileInfo=[]
	FileOffset=HdrSize
	for n in FileList:
		FileSize=os.path.getsize(SrcDir+'\\'+n)
		FileInfo.append((n, FileOffset, FileSize))
		FileOffset+=FileSize
	for n in FileInfo:
		Header+=n[0]+'\0'*(64-len(n[0]))+struct.pack('III',n[1],n[2],n[2])
	PakFile=open(PakFile,'wb')
	PakFile.write(Header)
	print "Packing..."
	for n in FileList:
		PakFile.write(open(SrcDir+'\\'+n,'rb').read())
	print "%d files.\nFinished." % len(FileList)
	sys.exit()

if len(sys.argv)<2: Usage()
if len(sys.argv)==2: PACPackage(sys.argv[1], sys.argv[1]+'.pac')
else: PACPackage(sys.argv[1], sys.argv[2])

ECHO = '''>>nul
:END
rem '''