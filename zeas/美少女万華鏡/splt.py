import os,sys

mbcode = 'shiftjis'
#mbcode = 'gbk'

def listunifile(filename):
	f = open(filename,'rb').read()
	if f[:2]=='\xff\xfe': f = f.decode('utf16')
	elif f[:2]=='\xfe\xff': f = f[2:].decode('utf_16_be')
	elif f[:3]=='\xef\xbb\xbf': f = f[3:].decode('utf8')
	else: f = f.decode(mbcode)
	return f.replace('\r\n','\n').split(u'\n')

def LoadName():
	NameMap={}
	name=[]
	newname=[]
	namefile = sys.path[0]+'\\name.txt'
	if os.path.isfile(namefile):
		name = listunifile(namefile)
	newnamefile = sys.path[0]+'\\newname.txt'
	if os.path.isfile(newnamefile):
		newname = listunifile(newnamefile)
	else:
		newname = listunifile(namefile)
	for n in range(min(len(name),len(newname))):
		NameMap[name[n]] = newname[n]
	return NameMap

def SaveName(nameMap):
	namefile = sys.path[0]+'\\name.txt'
	namelist = nameMap.keys()
	namelist.sort()
	open(namefile,'wb').write('\r\n'.join(namelist).encode('utf16'))

def findText(line):
	found = False
	for n in range(len(line)):
		c = line[n]
		if ord(c)>0x80:
			startpos = n
			found = True
			break
	if not found:
		return False
	for n in range(len(line),0, -1):
		c = line[n-1]
		if ord(c)>0x80:
			endpos = n
			break
	return startpos, endpos

charIgnore = {
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
    u'•' : u'·',
    u'・' : u'·',
    u'♪': u'♪',
}

def getDisp(line):
	displine = []
	for c in line:
		if c not in charIgnore: break
		displine.append(charIgnore[c])
	return ''.join(displine)

ignoreLine = [u'//', u'％',u'^cdel',u'^cbase',u'^iload',u'^cface',u'^sload',\
		u'^ffade',u'^cd',u'^cadd',u'^ce',u'^cinit',u'\\li',u'^eeffect',u'\\l,"',u'\\sub,@@!Avg_CharaGetNo,']
def checkIgnoreLine(line):
	chkline = line.strip()
	for chk in ignoreLine:
		if chkline.startswith(chk): return True
	return False

nameLine = [u'【']
def checkName(line):
	chkline = line.strip()
	for chk in nameLine:
		if chkline.startswith(chk):
			return [0,len(line)]
	return False

textLine = ['^savedate','^saveroute','^savescene',u'^select']
def checkTextLine(line):
	chkline = line.strip()
	for chk in textLine:
		if chkline.startswith(chk):
			return findText(line)
	textpos = findText(line)
	if textpos:
		if textpos[0]:
			print 'found unexpected text:'
			print line.encode('gb18030')
		else:
			return textpos
	return False

def SpltS(filename):
	TEXT=[]
	SPLT=[]
	NameMap=LoadName()
	counter = 1
	lastname = ''
	for line in listunifile(filename):
		if checkIgnoreLine(line):
			SPLT.append(line)
			continue
		namepos = checkName(line)
		if namepos:
			p, q = namepos
			name = line.strip()[p:q]
			line = line[:p]+'<NaMe>'+name+'</nAmE>'+line[q:]
			if name not in NameMap:
				NameMap[name] = name
			SPLT.append(line)
			lastname = name
			continue
		textpos = checkTextLine(line)
		if not textpos:
			SPLT.append(line)
			continue
		p, q = textpos
		if lastname:
			text = u'○%d %s○'%(counter, lastname)
			disp = u'●%d %s●'%(counter, lastname)
			lastname = ''
		else:
			text = u'○%d○'%counter
			disp = u'●%d●'%counter
		text += line[p:q]
		disp += getDisp(line[p:q])
		line = line[:p] + '<TeXt>%d</tExT>'%counter + line[q:]
		SPLT.append(line)
		TEXT.append(text)
		TEXT.append(disp)
		TEXT.append(u';'+u'='*63)
		counter += 1
	SaveName(NameMap)
	open(os.path.splitext(filename)[0]+'.txt','wb').write(u'\r\n'.join(TEXT).encode('utf16'))
	open(os.path.splitext(filename)[0]+'.splt','wb').write(u'\r\n'.join(SPLT).encode('shiftjis'))

SpltS(sys.argv[1])