import os
import sys
import pdb
import struct

def CalcStrLen(Src):
	pSrc=0
	while Src[pSrc]!='\0': pSrc+=1
	return pSrc

TXTFile=open(sys.argv[1]+'.txt','w')
Src=open(sys.argv[1],'rb').read()
pB=Src.find('\x0B\0',0x14)
while pB+1:
	pB+=2
	StrLen=struct.unpack('I',Src[pB:pB+4])[0]-1
	if StrLen==CalcStrLen(Src[pB+4:]):
		pB+=4
		pS=0
		IsFullChar=0
		while pS<StrLen:
			if ord(Src[pB+pS])>0x80:
				IsFullChar=1
				break
			pS+=1
		if IsFullChar:
			TXTFile.write('<%05X, %03d> %s\n' % (pB, StrLen, Src[pB:pB+StrLen]))
		pB+=StrLen
	pB=Src.find('\x0B\0',pB)








