@echo off
rem ='''
set pypath="%~dp0python\python.exe"
if not exist %pypath% set pypath="D:\tools\python-2.6.5\python.exe"
if not exist %pypath% ( echo I can't find "%~dp0python\python.exe" !
pause
goto :EOF
)
%pypath% -x "%~f0" %*
goto :EOF
rem '''

import os
import sys
import ctypes
#import pdb

keepcomment=0
isdoubleline=0
#^^^^for chinese ver

def openText(filename):
	pfile=open(filename,'rb')
	magic=pfile.read(3)
	pfile.seek(0)
	if magic[:2]=='\xff\xfe':
		text = pfile.read().decode('utf16')
	elif magic[:2]=='\xfe\xff':
		text = pfile.read()[2:].decode('utf_16_be')
	elif magic=='\xef\xbb\xbf':
		text = pfile.read().decode('utf8')[1:]
	else:
		text = pfile.read().decode('shiftjis')
	return text.replace('\r\n','\n').split('\n')

def SpltS(filename):
	TEXT=[]
	SPLT=[]
	flag=0
	count=1
	pretext=''
	prename=''
	for src in openText(filename+'.s'):
		tmp=src.strip()
		if tmp[:2]=='//':
			if keepcomment: SPLT.append(src)
			continue
		elif not tmp or tmp[0] in '^@\\;':
			if flag:
				if name: TEXT.append(u'\u25cf%d %s\u25cf\r\n'%(count, name))
				else:    TEXT.append(u'\u25cf%d\u25cf\r\n'%count)
				count+=1
				flag=0
			SPLT.append(src)
			continue
		if isdoubleline: #for chinese
			if not flag:
				if ord(tmp[0])<0x80 and tmp[0]!='[':
					try: voice,name,pretext=tmp.split(',', 2)
					except:
						pdb.set_trace()
					prename=name
					TEXT.append(u'\u25cb%d %s\u25cb%s'%(count, name, pretext))
				else:
					TEXT.append(u'\u25cb%d\u25cb%s'%(count, tmp))
					name=''
				flag=1
				continue #skip this line
			flag=0
			if pretext: tmp='\n'+tmp
		if ord(tmp[0])<0x80 and tmp[0] != '[':
			tmp=tmp.strip()
			if ord(tmp[0])>0x80:
				if u'\u300c' in tmp[1:]: tmp='v,'+tmp
				else: tmp='v,,'+tmp
			try: voice,name,text=tmp.split(',',2)
			except:
				try: name,text=tmp.split(',',1)
				except: pdb.set_trace()
			if pretext:
				if pretext[0]==u'\u300c' and text[0]!=u'\u300c': text=u'\u300c'+text
				if pretext[-1]==u'\u300d' and text[-1]!=u'\u300d': text+=u'\u300d'
			if not name and prename: name = prename
			pretext=''
			prename=''
			SPLT.append(voice+',<%04d>'%count)
			if isdoubleline:
				TEXT.append(u'\u25cf%d %s\u25cf%s\r\n'%(count, name, text))
			else:
				TEXT.append(u'\u25cb%d %s\u25cb%s\r\n\u25cf%d %s\u25cf\r\n'%(count, name, text, count, name))
			count+=1
		else:
			SPLT.append('<%04d>'%count)
			if isdoubleline:
				TEXT.append(u'\u25cf%d\u25cf%s\r\n'%(count, tmp))
			else:
				TEXT.append(u'\u25cb%d\u25cb%s\r\n\u25cf%d\u25cf\r\n'%(count, tmp, count))
			name=''
			count+=1
	open(filename+'.splt','wb').write(u'\r\n'.join(SPLT).encode('utf16'))
	open(filename+'.txt','wb').write(u'\r\n'.join(TEXT).encode('utf16'))

def JoinS(filename):
	FILELOG=[]
	UNIFIX=[0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x2019, 0xb7, 0x3016, 0x3017, 0x2026, 0x2014, 0xffe5, 0x203b, 0xb1, 0x2248, 0x2261, 0x2260, 0x2237, 0x2264, 0x2265, 0x222b, 0x3000, 0xff01, 0xff02, 0xff03, 0xff04, 0xff05, 0xff3e, 0xff07, 0xff08, 0xff09, 0xff0a, 0xff0b, 0xff0c, 0xff0d, 0xff0e, 0xff0f, 0xff10, 0xff11, 0xff12, 0xff13, 0xff14, 0xff15, 0xff16, 0xff17, 0xff18, 0xff19, 0xff1a, 0xff1b, 0xff1c, 0xff1d, 0xff1e, 0xff1f, 0xff20, 0xff21, 0xff22, 0xff23, 0xff24, 0xff25, 0xff26, 0xff27, 0xff28, 0xff29, 0xff2a, 0xff2b, 0xff2c, 0xff2d, 0xff2e, 0xff2f, 0xff30, 0xff31, 0xff32, 0xff33, 0xff34, 0xff35, 0xff36, 0xff37, 0xff38, 0xff39, 0xff3a, 0xff3b, 0xff3c, 0xff3d, 0xff3e, 0xff3f, 0xff40, 0xff41, 0xff42, 0xff43, 0xff44, 0xff45, 0xff46, 0xff47, 0xff48, 0xff49, 0xff4a, 0xff4b, 0xff4c, 0xff4d, 0xff4e, 0xff4f, 0xff50, 0xff51, 0xff52, 0xff53, 0xff54, 0xff55, 0xff56, 0xff57, 0xff58, 0xff59, 0xff5a, 0xff5b, 0xff5c, 0xff5d, 0xff5e, 0x25a0] + range(128, 0xFFFF)
	UNIFIX[0x2212]=u'\uff0d'
	UNIFIX[0x266A]=0x222E
	UNIFIX[0x301C]=0xFF5E
	UNIFIX[0x30fb]=u'\u00b7'
	UNIFIX[0xff65]=u'\u00b7'
	UNIFIX[0xff66]=0x30F2
	UNIFIX[0xff67]=0x30A1
	UNIFIX[0xff68]=0x30A3
	UNIFIX[0xff69]=0x30A5
	UNIFIX[0xff6A]=0x30A7
	UNIFIX[0xff6B]=0x30A9
	UNIFIX[0xff6C]=0x30E3
	UNIFIX[0xff6D]=0x30E5
	UNIFIX[0xff6E]=0x30E7
	UNIFIX[0xff6F]=0x30C3
	UNIFIX[0xff70]=0x30FC
	UNIFIX[0xff71]=0x30a2
	UNIFIX[0xff72]=0x30a4
	UNIFIX[0xff73]=0x30a6
	UNIFIX[0xff74]=0x30a8
	UNIFIX[0xff75]=0x30aa
	UNIFIX[0xff76]=0x30ab
	UNIFIX[0xff77]=0x30ad
	UNIFIX[0xff78]=0x30af
	UNIFIX[0xff79]=0x30b1
	UNIFIX[0xff7a]=0x30b3
	UNIFIX[0xff7b]=0x30b5
	UNIFIX[0xff7c]=0x30b7
	UNIFIX[0xff7d]=0x30b9
	UNIFIX[0xff7e]=0x30bb
	UNIFIX[0xff7f]=0x30bd
	UNIFIX[0xff80]=0x30bf
	UNIFIX[0xff81]=0x30c1
	UNIFIX[0xff82]=0x30c4
	UNIFIX[0xff83]=0x30c6
	UNIFIX[0xff84]=0x30c8
	UNIFIX[0xff85]=0x30ca
	UNIFIX[0xff86]=0x30cb
	UNIFIX[0xff87]=0x30cc
	UNIFIX[0xff88]=0x30cd
	UNIFIX[0xff89]=0x30ce
	UNIFIX[0xff8a]=0x30cf
	UNIFIX[0xff8b]=0x30d2
	UNIFIX[0xff8c]=0x30d5
	UNIFIX[0xff8d]=0x30d8
	UNIFIX[0xff8e]=0x30db
	UNIFIX[0xff8f]=0x30de
	UNIFIX[0xff90]=0x30df
	UNIFIX[0xff91]=0x30e0
	UNIFIX[0xff92]=0x30e1
	UNIFIX[0xff93]=0x30e2
	UNIFIX[0xff94]=0x30e4
	UNIFIX[0xff95]=0x30e6
	UNIFIX[0xff96]=0x30e8
	UNIFIX[0xff97]=0x30e9
	UNIFIX[0xff98]=0x30ea
	UNIFIX[0xff99]=0x30eb
	UNIFIX[0xff9a]=0x30ec
	UNIFIX[0xff9b]=0x30ed
	UNIFIX[0xff9c]=0x30ef
	UNIFIX[0xff9d]=0x30f3
	UNIFIX[0xff9e]=0x309b
	UNIFIX[0xff9f]=0x309c
	
	TEXT={}
	for n in open(filename+'.txt', 'rb').read().decode('utf16').replace(u'\r\n',u'\n').replace(u'\r',u'\n').split('\n'):
		if not n: continue
		if n[0]   == u'\u25cb':
			p=1
			while n[p] in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']: p+=1
			Count=int(n[1:p])
			if u'\u25cb' not in n[p:]:
				FILELOG.append(u'Lost "\u25cb" @ %d'%Count)
				text = name = u'NULL'
			else:
				text = n[n.index(u'\u25cb', p)+1:].replace('\\n', '\r\n')
				name = n[p:n.index(u'\u25cb', p)].strip()
			TEXT[Count]=[name.translate(UNIFIX),text.translate(UNIFIX)]
			flag=0
		elif n[0] == u'\u25cf':
			p=1
			while n[p] in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']: p+=1
			Count=int(n[1:p])
			if u'\u25cf' not in n[p:]:
				FILELOG.append(u'Lost "\u25cf" @ %d'%Count)
				text = name = u'NULL'
			else:
				text = n[n.index(u'\u25cf', p)+1:].replace('\\n', '\r\n')
				name = n[p:n.index(u'\u25cf', p)].strip().translate(UNIFIX)
				if u'\uff20' in TEXT[Count][0]:
					name=name.split(u'\uff20')[0].encode('gbk')+\
						(u'\uff20'+TEXT[Count][0].split(u'\uff20')[1]).encode('shiftjis')
				elif name:
					try: name=name.encode('gbk')+(u'\uff20'+TEXT[Count][0]).encode('shiftjis')
					except: pdb.set_trace()
			if text: TEXT[Count]=[name,text.translate(UNIFIX)]
			elif TEXT[Count][0]:    TEXT[Count][0]=name
			flag=1
	if not flag:
		print 'trans cross'
		pdb.set_trace()
	if FILELOG:
		print FILELOG
		pdb.set_trace()
	
	SSCR=[]
	for n in open(filename+'.splt','rb').read().decode('utf16').split('\r\n'):
		if not n or n.strip()[0] in '\\/@^':
			try: SSCR.append(n.encode('shiftjis'))
			except: pdb.set_trace()
			continue
		elif '<' in n:
			p=n.index('<')
			if n[p+5]!='>':
				SSCR.append(n.encode('shiftjis'))
				continue
			count=int(n[p+1:p+5])
			name, text=TEXT[count]
			if name:
				t=n[:p].encode('gbk')
				try: t+='%s,%s'%(name,text.encode('gbk'))
				except: pdb.set_trace()
				t+=n[p+6:].encode('gbk')
			else:
				t=(n[:p]+text+n[p+6:]).encode('gbk')
			SSCR.append(t)
		else:
			pdb.set_trace()
	if count!=len(TEXT):
		print 'less count.'
		pdb.set_trace()
	open(filename+'.s.out','w').write('\n'.join(SSCR))

def JoinText():
	TORG={}
	TNEW={}
	FILELOG=[]
	for n in open(sys.argv[1], 'rb').read().decode('utf16').split(u'\r\n'):
		if not n: continue
		elif n[0]   == u'\u25cb':
			p=1
			while n[p] in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']: p+=1
			Count=int(n[1:p])
			if u'\u25cb' not in n[p:]:
				FILELOG.append(u'Lost "\u25cb" @ %d'%Count)
				text = name = u'NULL'
			else:
				TORG[Count]=n
	for n in open(sys.argv[2], 'rb').read().decode('utf16').split(u'\r\n'):
		if not n: continue
		elif n[0] == u'\u25cf':
			p=1
			while n[p] in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']: p+=1
			Count=int(n[1:p])
			if u'\u25cf' not in n[p:]:
				FILELOG.append(u'Lost "\u25cf" @ %d'%Count)
				text = name = u'NULL'
			else:
				TNEW[Count]=n
	COUNT=[n for n in TORG]
	COUNT.sort()
	TEXT=[]
	for n in COUNT:
		TEXT.append(TORG[n])
		TEXT.append(TNEW[n])
		TEXT.append(u'')
	open(sys.argv[1], 'wb').write(u'\r\n'.join(TEXT).encode('utf16'))

filename, ext = os.path.splitext(sys.argv[1])
if len(sys.argv)>2:
	JoinText()
elif ext.lower() == '.s':
	SpltS(filename)
elif ext.lower() in ['.txt', '.splt']:
	JoinS(filename)
else:
	print 'What can I do ?'

