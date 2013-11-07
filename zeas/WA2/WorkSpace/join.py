#import pdb
import os,sys
from fontutil import *
from fontmap import *
from patch import patch

charmap = eval(open(sys.path[0]+'\\charmap.txt').read())
isfontinit = 0

suffixChar = {
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
    u'〜': u'～',
    u'」': u'」',
    u'』': u'』',
    u'）': u'）',
    u'\\': u'\\',
    u'k' : u'k',
    u'•' : u'·',
    u'・' : u'·',
    u'♪': u'♪',
}

def textlen(s):
	num = 0
	for c in s:
		if ord(c)>0x80:
			num+=1
	return num

def listunifile(filename):
	f = open(filename,'rb').read()
	if f[:2]=='\xff\xfe': f = f.decode('utf16')
	elif f[:2]=='\xfe\xff': f = f[2:].decode('utf_16_be')
	elif f[:3]=='\xef\xbb\xbf': f = f[3:].decode('utf8')
	else: f = f.decode('gb18030')
	return f.replace('\r\n','\n').replace('\r','').split(u'\n')

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

nameMap = LoadName()
text=[]
src=listunifile(sys.argv[1])
pLine = 0
print 'importing %s ...'%os.path.split(sys.argv[1])[1]
while pLine<len(src):
	line = src[pLine]
	pLine += 1
	if line and line[0] in u'●○':
		sig = line[0]
		sigline = line
		label = line.split(sig)[1]
		if label == 'name':
			name = src[pLine]
			if name not in nameMap:
				print 'name lost: '+name.encode('gb18030')
				os.system('pause')
				sys.exit()
			text.append([nameMap[name]])
			pLine+=1
			continue
		sentance = []
		line = src[pLine]
		while not(line and line[0] in u'●○◆'):
			if sig == u'○' and not line:
				#print '1?'
				pass
			sentance.append(line)
			pLine += 1
			line = src[pLine]
		if sig == u'●':#句尾补全
			origsen = text[-1]
			displine = ''
			if(len(sentance)==len(origsen)):
				for n in range(len(origsen)):
					o = origsen[n]
					l = sentance[n]
					p = -1
					t = 0
					linelen = len(o)
					while linelen >= -p and o[p] in suffixChar and l[-1] != o[p]:
						p -= 1
					if p+1: l += o[p+1:]
					if textlen(l)>28:
						#displine = u'太长：━━━━━━━━28个字，参照这条线━━━━━━━━'
						pass
					sentance[n] = l
			else:
				displine = u'行数不一致：'
			if displine:
				print displine
				print sig + label
				print '\n'.join(origsen)
				print u'━'*28
				print '\n'.join(sentance)
				print
			text[-1] = sentance
		else:
			text.append(sentance)
filename = os.path.splitext(os.path.split(sys.argv[1])[1])[0]
if filename in patch:
	text.append([patch[filename]])

def saveFont():
	FONT80MAIN.save(sys.path[0]+'\\output\\FONT801.tga')
	FONT80SHAD.save(sys.path[0]+'\\output\\FSHD801.tga')
	FONT14MAIN.save(sys.path[0]+'\\output\\FONT141.tga')
	FONT14SHAD.save(sys.path[0]+'\\output\\FSHD141.tga')
	open(sys.path[0]+'\\charmap.txt','w').write(charmap.__repr__())
	print 'tga fonts renewed.'

def initFont():
	for n in range(32,128):
		n = unichr(n)
		if n in fontmap:
			del fontmap[n]
	for n in charmap:
		if charmap[n] in fontmap:
			del fontmap[charmap[n]]
#		else:
#			print 'fontmap overflow : U%04X!'%ord(n)
	global FONT80MAIN, FONT80SHAD, FONT14MAIN, FONT14SHAD
	FONT80MAIN = Image.open(sys.path[0]+'\\output\\FONT80.tga')
	FONT80MAIN.load()
	FONT80SHAD = Image.open(sys.path[0]+'\\output\\FSHD80.tga')
	FONT80SHAD.load()
	FONT14MAIN = Image.open(sys.path[0]+'\\output\\FONT14.tga')
	FONT14MAIN.load()
	FONT14SHAD = Image.open(sys.path[0]+'\\output\\FSHD14.tga')
	FONT14SHAD.load()
	FontFace = sys.path[0]+'\\font.TTF'
	if 2 != ctypes.windll.gdi32.AddFontResourceExA(FontFace, 16, 0):
		print 'add font fail: '+FontFace
		os.system('pause')
		sys.exit()
	global hdc80, hdc14, tm80, tm14, isfontinit, doubleBrightness14, doubleBrightness80
	BrightFactor = 4.
	doubleBrightness80 = ''.join([chr(int(n*BrightFactor)) for n in range(int(256/BrightFactor))])+'\xFF'*(256-int(256/BrightFactor))
	BrightFactor = 8.
	doubleBrightness14 = ''.join([chr(int(n*BrightFactor)) for n in range(int(256/BrightFactor))])+'\xFF'*(256-int(256/BrightFactor))
	hdc80, tm80 = createFont(28, u'幼圆')
	hdc14, tm14 = createFont(11, u'幼圆')
	isfontinit = 1

def newChar(c): #appendNewChar
	global charmap, FONT80MAIN, FONT80SHAD, FONT14MAIN, FONT14SHAD
	if not isfontinit:
		initFont()
		global averFilter
		averFilter = ctypes.WinDLL(sys.path[0]+'\\averageFilter.dll').averageFilter
	f = fontmap.keys()[-1]
	charmap[c]=f
	x,y = fontmap[f]
	del fontmap[f]
	glp80 = RenderFont(c, hdc80, 40, tm80, doubleBrightness80, 5, 4)
	glp14 = RenderFont(c, hdc14, 24, tm14, doubleBrightness14, 5, 5)
	tmp = '\0'*(48*48)
	r,g,b,a = glp80.split()
	averFilter(a.tostring(), 40, 2, tmp)
	FONT80MAIN.paste(glp80, (x*40, y*40))
	FONT80SHAD.paste(Image.merge('RGBA', (r,g,b,Image.fromstring('L', (40,40), tmp))), (x*40, y*40))
	r,g,b,a = glp14.split()
	averFilter(a.tostring(), 24, 1, tmp)
	FONT14MAIN.paste(glp14, (x*24, y*24))
	FONT14SHAD.paste(Image.merge('RGBA', (r,g,b,Image.fromstring('L', (24,24), tmp))), (x*24, y*24))
	print c.encode('gb18030'),
	return f

def applyCharMap(s):
	ret = []
	for c in s:
		if c in charmap:
			c = charmap[c]
		elif ord(c) < 0x80:
			c = str(c)
		else:
			c = newChar(c)
		ret.append(c)
	return ''.join(ret)

open(sys.argv[2],'wb').write(applyCharMap(','.join(['\\n'.join(n) for n in text])))
if isfontinit: saveFont()
