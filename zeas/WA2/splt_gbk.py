import os,sys

def isText(s):
	for c in s:
		if ord(c)>0x2000:
			return True
	return False

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
    u'」': u'」',
    u'』': u'』',
    u'）': u'）',
    u'\\': u'\\',
    u'k' : u'k',
}

lastline = ''
namepath = sys.path[0]+'\\name.txt'
namelist = open(namepath,'rb').read().decode('utf16').replace('\r\n','\n').replace('\r','\n').split('\n')
text = []
nLine = 0
updateName = 0
igline = []
#print 'namelist:\n-------\n'+u'\n'.join(namelist).encode('gbk')+'\n-------\n'

def mayName(s):
	if s[:1] not in u'「『':
		return False
	for c in u'“「『…―。」\\':
		if c in lastline:
			return False
	return True

for line in open(sys.argv[1],'rb').read().decode('gbk').split(','):
	nLine += 1
	if not isText(line):
		igline.append(line)
		lastline = ''
		continue
	elif line and line in namelist:
		if igline:
			text.append(u'○%d○;'%nLine + u'-'*64 + '\n' + ','.join(igline))
			igline = []
		text.append(u'○name○;' + u'━'*31 + '\n' + line)
		lastline = line
		continue
	elif mayName(line):
		if lastline and len(lastline)<8 and lastline not in namelist:
			#namelist.append(lastline)
			print 'new name ' + lastline.encode('gb18030')
			#updateName = 1
			#os.system('pause')
		elif lastline and len(lastline)<16 and lastline not in namelist:
			print 'may name ' + lastline.encode('gb18030')
			#os.system('pause')
	lineorig = line.split('\\n')
	linedisp = []
	for s in lineorig:
		p = 0
		while p<len(s) and s[p] in prefixIgnore:
			p+=1
		linedisp.append(s[:p])
	if igline:
		text.append(u'○%d○;'%nLine + u'-'*64 + '\n' + ','.join(igline))
		igline = []
	if linedisp == lineorig:
		print line.encode('gb18030')+' ignored.'
		text.append(u'○%d○;'%nLine + u'━'*32 + '\n' + line)
	else:
		text.append(u'○%d○;'%nLine + u'■'*32 + '\n' + '\n'.join(lineorig))
		text.append(u'●%d●;'%nLine + u'━'*32 + '\n' + '\n'.join(lineorig))
		#text.append(u'◆;■■' + u'■'*32)
	lastline = line
if igline:
	patch = open(sys.path[0]+'\\patch.txt').read()+'\t'+\
	os.path.splitext(os.path.split(sys.argv[1])[1])[0].__repr__()+' : '+','.join(igline).__repr__()+',\n'
	open(sys.path[0]+'\\patch.txt','w').write(patch)
text.append(u'◆;end of text')
namelist.sort()
if updateName: open(namepath,'wb').write(u'\n'.join(namelist).replace('\n','\r\n').encode('utf16'))
open(sys.argv[1][:-4]+'.inf','wb').write(u'\n'.join(text).replace('\n','\r\n').encode('utf16'))
