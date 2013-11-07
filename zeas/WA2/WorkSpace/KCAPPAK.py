import os
import sys
import struct

def lzss_uncompress(compr, comprlen, uncomprlen):
	act_uncomprlen = 0
	curbit = 0
	nCurWindowByte = 0xfee
	win = ['\0']*4096
	flag = 0
	uncompr = []
	comprlen += compr.tell()
	while compr.tell() < comprlen:
		flag >>= 1
		if (compr.tell() >= comprlen or len(uncompr)>=uncomprlen): break
		if not(flag & 0x0100):
			flag = ord(compr.read(1)) | 0xff00
			print '%.1f%%\r' % (len(uncompr)*100./uncomprlen),
		if flag & 1:
			data = compr.read(1)
			uncompr.append(data)
			win[nCurWindowByte&0xFFF]=data
			nCurWindowByte+=1
		else:
			win_offset = ord(compr.read(1))
			copy_bytes = ord(compr.read(1))
			win_offset |= (copy_bytes & 0xf0) << 4
			copy_bytes = (copy_bytes&0x0f)+3
			for i in range(copy_bytes):
				data = win[(win_offset + i) & 0xFFF]
				uncompr.append(data)
				win[nCurWindowByte&0xFFF] = data
				nCurWindowByte+=1
	return ''.join(uncompr)

def UnpPAK():
	filetype, filename, d, d, offset, filesize = range(6) #FileInfo
	PAK=open(sys.argv[1],'rb')
	if PAK.read(4)!='KCAP':
		print 'Only support KCAP Pak.'
		sys.exit()
	unknown, off, famount = struct.unpack('III', PAK.read(12))
	FileInfos = [struct.unpack('I24sIIII',PAK.read(0x2C)) for n in range(famount)]
	Path=os.path.splitext(sys.argv[1])[0]
	if not os.path.isdir(Path): os.mkdir(Path)
	Path+='\\'
	for FileInfo in FileInfos:
		print FileInfo[filename].strip('\0')
	sys.exit()
	for FileInfo in FileInfos:
		PAK.seek(FileInfo[offset])
		if FileInfo[filetype]==1:
			comprlen, uncomprlen = struct.unpack('II', PAK.read(8))
			Data = lzss_uncompress(PAK, comprlen, uncomprlen)
		elif FileInfo[filetype]==0:
			Data = PAK.read(FileInfo[filesize])
		print FileInfo[filename].strip('\0')
		open(Path+FileInfo[filename].strip('\0'),'wb').write(Data)

def PckPAK():
	if(len(sys.argv)>2):
		outfile = sys.argv[2]
	else:
		outfile = sys.argv[1]+'.out.pak'
	FileList=[]
	for n in os.listdir(sys.argv[1]):
		if os.path.isfile(sys.argv[1]+'\\'+n): FileList.append(n)
	Hdr=['KCAP\xcd\xe8\xbb\xa7\xba\xda\0\0'+struct.pack('I', len(FileList))]
	offset=0x2c*len(FileList)+16
	for n in FileList:
		CmprFlag='\0\0\0\0'
		FileName = n
		if os.path.splitext(n)[1].lower() in ['.uci', '.zgb', '.zgf']:
			FileName=os.path.splitext(n)[0]+'.tga'
			CmprFlag='\1\0\0\0'
		FileSize=os.path.getsize(sys.argv[1]+'\\'+n)
		FileName=FileName.decode('gbk').encode('shiftjis').lower()+'\0'*(24-len(FileName))
		Hdr.append(CmprFlag+FileName+'\xcd\xe8\xbb\xa7\xba\xda\0\0'+struct.pack('II', offset, FileSize))
		offset+=FileSize
	PAK=open(outfile,'wb')
	PAK.write(''.join(Hdr))
	for n in FileList:
		PAK.write(open(sys.argv[1]+'\\'+n,'rb').read())
	print outfile+' created'

#UnpPAK()
if len(sys.argv)<2:
	print "LEAF KCAP 3.0 Packer v0.1\nUsage: %s <DIR>\nNotice: This tool only pack all the file but directory in specified DIR." % os.path.split(sys.argv[0])[1]
	sys.exit()
PckPAK()