import os
import sys
import struct

def Usage():
	print "Giga VGNeo .BIN Importer\nUsage:\n%s <TXTFile> <BINFile> [OUTFile]" % os.path.split(sys.argv[0])[1]
	sys.exit()

def ImportTxt(TXTFile, BINFile, OUTFile):
	TXTFile=open(TXTFile,'r')
	OUTFile=open(OUTFile,'wb')
	BINData=open(BINFile,'rb').read()
	TXT=TXTFile.readline()
	while TXT:
		if TXT[0]=='\n':
			TXT=TXTFile.readline()
			continue
		elif TXT[0]!='<':
			print "Invalid Text Format!"
			sys.exit()
		Offset=int(TXT[1:TXT.find(',')],16)
		StrLen=int(TXT[TXT.find(',')+1:TXT.find('>',2)])
		Text=TXT[TXT.find('> ')+2:-1]
		if len(Text)>StrLen:
			print "Too Long Text, Text may be cut down."
			Text=Text[:StrLen]
		BINData=BINData[:Offset]+Text+'\0'+BINData[Offset+len(Text)+1:]
		TXT=TXTFile.readline()
	OUTFile.write(BINData)

if len(sys.argv)<3: Usage()
if len(sys.argv)==3: ImportTxt(sys.argv[1], sys.argv[2], sys.argv[2]+'.out.bin')
else: ImportTxt(sys.argv[1], sys.argv[2], sys.argv[3])