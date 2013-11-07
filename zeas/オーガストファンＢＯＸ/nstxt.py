import os,sys,random
import ctypes
from pdb import set_trace as int3

keepcomment = 1

def listunifile(filename):
	f = open(filename,'rb').read()
	if f[:2]=='\xff\xfe': f = f.decode('utf16')
	elif f[:2]=='\xfe\xff': f = f[2:].decode('utf_16_be')
	elif f[:3]=='\xef\xbb\xbf': f = f[3:].decode('utf8')
	else: f = f.decode('gb18030')
	return f.replace('\r\n','\n').split(u'\n')

prefixIgnore = {
    u'　': u'　',
    u'「': u'「',
    u'『': u'『',
    u'（': u'（',
    u'！': u'！',
    u'？': u'？',
    u'…': u'…',
    u'—': u'—',
    u'─': u'—',
    u'，': u'，',
    u'。': u'。',
    u'～': u'～',
}
suffixIgnore = {
    u'　': u'　',
    u'」': u'」',
    u'』': u'』',
    u'）': u'）',
    u'！': u'！',
    u'？': u'？',
    u'…': u'…',
    u'—': u'—',
    u'─': u'—',
    u'，': u'，',
    u'。': u'。',
    u'～': u'～',
    u'\\': u'\\',
}

def LoadName():
	NameMap={}
	name=[]
	newname=[]
	namefile = sys.path[0]+'\\name.txt'
	if os.path.isfile(namefile):
		name = listunifile(namefile)
	namefile = sys.path[0]+'\\newname.txt'
	if os.path.isfile(namefile):
		newname = listunifile(namefile)
	for n in range(min(len(name),len(newname))):
		NameMap[name[n]] = newname[n]
	return NameMap

def LoadText(filename):
	f = listunifile(filename)
	ret = {}
	nLine = 0
	TooLongLog = []
	while nLine<len(f):
		line = f[nLine]
		nLine += 1
		if not line or line[0] not in u'○●':
			continue
		sym=line[0]
		p=line.find(sym,1)
		if p==-1:
			print u'line: %s lost ●'%line
			continue
		q=line.find(sym,p+1)
		if q==-1:
			print u'line: %s lost ●'%line
			continue
		try: count = int(line[p+1:q])
		except:
			print u'line: %s fail to get count'%line
			continue
		opt = line[q+1:]
		if ';' in opt:
			opt = opt[:opt.index(';')]
		line = f[nLine]
		text = []
		while 1:
			if line and line[0] in u'○●' and line.count(line[0]) == 3: break
			text.append(line)
			nLine += 1
			if nLine>=len(f): break
			line = f[nLine]
		if sym == u'●':
			orig = ret[count][1]
			if len(text)!=len(orig):
				text = u'\n'.join(text)
				orig = u'\n'.join(orig)
				if orig and orig[-1] == '\\' and (not text or text[-1] != '\\'):
					text += '\\'
				ret[count] = (opt, text)
				continue
			for n in range(len(orig)):
				origline = orig[n]
				line = text[n]
				if line and line[-1] == ' ':
					text[n] = line.rstrip(' ')
					continue
				if line==u'「' or not line:
					text[n] = origline
					continue
				p = -1
				linelen = min(len(origline), len(line))
				while linelen >= -p and origline[p] in suffixIgnore and line[-1] != origline[p]:
					p -= 1
				if p+1: line += origline[p+1:]
				if len(line)>21:
					#TooLongLog.append('%d '%count + line)
					pass
				text[n] = line
		ret[count] = (opt, text)
	if TooLongLog:
		print 'TooLong:'
		print '\r\n'.join(TooLongLog).encode('gb18030')
	return ret

preconvmap = {
	# u'A' : u"Ａ",
	# u'B' : u"Ｂ",
	# u'C' : u"Ｃ",
	# u'D' : u"Ｄ",
	# u'E' : u"Ｅ",
	# u'F' : u"Ｆ",
	# u'G' : u"Ｇ",
	# u'H' : u"Ｈ",
	# u'I' : u"Ｉ",
	# u'J' : u"Ｊ",
	# u'K' : u"Ｋ",
	# u'L' : u"Ｌ",
	# u'M' : u"Ｍ",
	# u'N' : u"Ｎ",
	# u'O' : u"Ｏ",
	# u'P' : u"Ｐ",
	# u'Q' : u"Ｑ",
	# u'R' : u"Ｒ",
	# u'S' : u"Ｓ",
	# u'T' : u"Ｔ",
	# u'U' : u"Ｕ",
	# u'V' : u"Ｖ",
	# u'W' : u"Ｗ",
	# u'X' : u"Ｘ",
	# u'Y' : u"Ｙ",
	# u'Z' : u"Ｚ",
	# u'[' : u"［",
	# u']' : u"］",
	# u'_' : u"＿",
	# u'`' : u"｀",
	# u'a' : u"ａ",
	# u'b' : u"ｂ",
	# u'c' : u"ｃ",
	# u'd' : u"ｄ",
	# u'e' : u"ｅ",
	# u'f' : u"ｆ",
	# u'g' : u"ｇ",
	# u'h' : u"ｈ",
	# u'i' : u"ｉ",
	# u'j' : u"ｊ",
	# u'k' : u"ｋ",
	# u'l' : u"ｌ",
	# u'm' : u"ｍ",
	# u'n' : u"ｎ",
	# u'o' : u"ｏ",
	# u'p' : u"ｐ",
	# u'q' : u"ｑ",
	# u'r' : u"ｒ",
	# u's' : u"ｓ",
	# u't' : u"ｔ",
	# u'u' : u"ｕ",
	# u'v' : u"ｖ",
	# u'w' : u"ｗ",
	# u'x' : u"ｘ",
	# u'y' : u"ｙ",
	# u'z' : u"ｚ",
	# u'.' : u'．',
	# u'-' : u'－',
	# u'0' : u"０",
	# u'1' : u"１",
	# u'2' : u"２",
	# u'3' : u"３",
	# u'4' : u"４",
	# u'5' : u"５",
	# u'6' : u"６",
	# u'7' : u"７",
	# u'8' : u"８",
	# u'9' : u"９",
#	u'♪' : u'\u222e',
	u'・' : u'·',
	u'〜' : u'～',
	# u'ー': u'—',
}

charmapfile = sys.path[0]+'\\charmap.txt'
if os.path.isfile(charmapfile):
	charmap = eval(open(charmapfile).read())
else:
	charmap = {
		u'\u3000': [u'\u3000'.encode('shiftjis').decode('gbk')],
	    u'♪': [u'♪'.encode('shiftjis').decode('gbk')],
	}
	for n in range(32, 127): charmap[unichr(n)] = [unichr(n)]
fc = [chr(n) for n in range(0x81,0xA0)]
sc = [chr(n) for n in (range(0x40,0x7f)+range(0x81,0xE1))]
used = {}
for t in charmap.values():
	for c in t:
		used[c] = 0
chars = [(fc[n/len(sc)]+sc[n%len(sc)]).decode('gbk') for n in range(len(fc)*len(sc))][1:]
chars = [c for c in chars if c not in used]
random.shuffle(chars)

CharStatistic = {}#字符概率分布
def StatAndUpper(s):
	global CharStatistic
	ret = []
	p = 0
	while p<len(s):
		c = s[p]
		p += 1
		# if c == '#':
			# c += s[p:p+6]
			# p += 6
			# ret.append(c)
			# continue
		# elif c == ':':
			# c += s[p:p+1]
			# p += 1
			# ret.append(c)
			# continue
		if c in preconvmap:
			c = preconvmap[c]
		if ord(c)>0x80:
			if c not in CharStatistic:
				CharStatistic[c] = 0
			CharStatistic[c] += 1
		ret.append(c)
	return ''.join(ret)

def UpdateCharMap(charmap):
	for c in CharStatistic:
		if c in charmap: continue
		count = CharStatistic[c]
		dst = []
		n = 1
		if count > 64:
			n += 1
		if count > 256:
			n += 1
		for n in range(n):
			dst.append(chars.pop())
		charmap[c] = dst

def ConvChar(s):
	ret = []
	for c in s:
		if ord(c)<0x80:
			ret.append(c)
			continue
		if c not in charmap:
			import pdb
			pdb.set_trace()
		ret.append(random.choice(charmap[c]))
	return ''.join(ret)

NameMap = LoadName()
Text = LoadText(sys.argv[2])
for count in Text:
	opt, t = Text[count]
	Text[count] = (opt, [StatAndUpper(n) for n in t])
for name in NameMap:
	StatAndUpper(name)
	StatAndUpper(NameMap[name])

charmaplen = len(charmap)
UpdateCharMap(charmap)

Text2 = {}
for count in Text:
	opt, t = Text[count]
	t = u'\n'.join(t)
	if opt != 'noenc':
		t = ConvChar(t)
	Text2[count] = t
Text = Text2

newscript = []
f = listunifile(sys.argv[1])
for nLine in xrange(len(f)):
	line = f[nLine]
	while '<NaMe>' in line:
		p = line.index('<NaMe>')
		q = line.find('</nAmE>',p+1)
		if q==-1:
			print u'errline @ %d: %s'%(nLine, line)
			break
		name = line[p+6:q]
		if name not in NameMap:
			print u'Lost name : %s' % name
		else:
			name = NameMap[name]
		line = line[:p] + ConvChar(name) + line[q+7:]
	while '<TeXt>' in line:
		p = line.index('<TeXt>')
		q = line.find('</tExT>',p+1)
		try: count = int(line[p+6:q])
		except:
			print u'errline @ %d: %s'%(nLine, line)
			break
		if count not in Text:
			print u'lost text %d'%count
			break
		line = line[:p] + Text[count] + line[q+7:]
	newscript.append(line)

def win32_unicode_argv(): 
	"""Uses shell32.GetCommandLineArgvW to get sys.argv as a list of Unicode strings.""" 
	GetCommandLineW = ctypes.windll.kernel32.GetCommandLineW 
	GetCommandLineW.restype = ctypes.c_wchar_p
	cmd = GetCommandLineW() 
	CommandLineToArgvW = ctypes.windll.shell32.CommandLineToArgvW 
	CommandLineToArgvW.argtypes = [ctypes.c_wchar_p, ctypes.POINTER(ctypes.c_long)] 
	CommandLineToArgvW.restype = ctypes.POINTER(ctypes.c_wchar_p) 
	argc = ctypes.c_int(0) 
	argv = CommandLineToArgvW(cmd, ctypes.byref(argc)) 
	if argc.value > 0:
		# Remove Python executable and commands if present 
		start = argc.value - len(sys.argv) 
		return [argv[i] for i in 
				xrange(start, argc.value)] 

open(sys.argv[3],'w').write(u'\n'.join(newscript).encode('gbk'))
if charmaplen != len(charmap):
	print 'new char found, charmap.txt & fonts updated'
	from fontTools import ttLib
	argv=win32_unicode_argv()
	basepath = os.path.split(argv[0])[0]
	fontspath = basepath+u'\\fonts'
	tj = ttLib.TTFont(sys.path[0]+"\\fonts\msgothic.ttc", fontNumber=0)
	filelist = []
	for filename in os.listdir(fontspath):
		if os.path.splitext(filename)[1].lower() == '.ttf':
			filelist.append(filename)
	for fontfilename in filelist:
		fontname = os.path.splitext(fontfilename)[0].encode('utf_16_be')
		tt = ttLib.TTFont(fontspath + '\\' + fontfilename, fontNumber=0)
		names = {}
		for n in range(len(tt['name'].names)):
			name = tt['name'].names[n]
			if name.string == fontname:#utf_16_be
				tt['name'].names[n].string = u'BinaryPot'.encode('utf_16_be')
			elif name.string.lower() in ('simhei', 'kaiti','simsun','fangsong'):
				tt['name'].names[n].string = 'BinaryPot'
		cmap = tt['cmap'].tables[-1].cmap
		nmap = {}
		glyf = tt['glyf'].glyphs
		ngly = {}
		glyphodr = tt.getGlyphOrder()
		for c in charmap:
			realchar = ord(c)
			for fakechar in charmap[c]:
				fakechar = ord(fakechar)
				if fakechar not in cmap:
					pdb.set_trace()
				if realchar==0x266a:#∮->♪
					name = nmap[fakechar] = tj['cmap'].tables[0].cmap[realchar]
					ngly[name] = tj['glyf'].glyphs[name]
					tt['glyf'].glyphs[name] = tj['glyf'].glyphs[name]
					if 'hmtx' in tt: tt['hmtx'].metrics[name] = tj['hmtx'].metrics[name]
					if 'vmtx' in tt: tt['vmtx'].metrics[name] = tj['vmtx'].metrics[name]
					glyphodr.append(name)
				elif realchar not in cmap:
					print repr(realchar), unichr(realchar).encode('gb18030'), 'not found in fonts'
				else:
					name = nmap[fakechar] = cmap[realchar]
					ngly[name] = glyf[name]
		tt['cmap'].tables[-1].cmap = nmap
		for n in tt['glyf'].glyphs.keys():
			if n[:3]=='uni' and n not in ngly:
				del tt['glyf'].glyphs[n]
				glyphodr.remove(n)
		if 'DSIG' in tt: del tt['DSIG']
		if 'prep' in tt: del tt['prep']
		if 'GSUB' in tt: del tt['GSUB']
		if 'EBLC' in tt: del tt['EBLC']
		if 'EBDT' in tt: del tt['EBDT']
		del tt['cvt ']
		tt.setGlyphOrder(glyphodr)
		tt.save(basepath+'\\newfont\\'+fontfilename)
		print fontfilename, 'saved.'
	open(charmapfile, 'w').write(repr(charmap))