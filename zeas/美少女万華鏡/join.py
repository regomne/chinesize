#import pdb
import os,sys

codefile, textfile, outfile = sys.argv[1:4]

ignoreChar = {
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

NameMap = LoadName()
Text={}
print 'importing %s ...'%os.path.split(textfile)[1]
for line in listunifile(textfile):
	if not line or line[0] in u';#': continue
	if line[0] in u'○●':
		if not line[0] in line[1:]:
			print 'lost %s:n\%s'%(line[0].encode('gb18030'),line.encode('gb18030'))
			os.system('pause')
		p=line.index(line[0],1)
		n = int(line[1:p].split()[0])
		origline=line
		line=line[p+1:]
		p=0
		#先判断是否有输入文本
		if origline[0] == u'●':
			while p<len(line) and line[p] in ignoreChar: p+=1
			if not line[p:] and line[:p] == Text[n][:p]: continue
		#替换名字
		while '{' in line:
			p = line.index('{')
			r = line.find('}', p)
			if r==-1:
				print 'Warning: %s lost "}"'%origline.enocde('gb18030')
				break
			name = line[p+1:r]
			if name not in NameMap:
				print 'Warning: Unregistered name "%s" in line %d'%(name,n)
				break
			line = line[:p] + NameMap[name] + line[r+1:]
		#补全括号
		if origline[0] == u'●':
			origline=Text[n]
			if origline and origline[-1] in ignoreChar:
				p=unichr(ord(origline[-1])-1)
				if line.count(p)>line.count(origline[-1]):
					line+=origline[-1]
		Text[n] = line

charMap = { u'♪': '\xfe\xfe', u'¶':'\x81\xf7'}
def convGBK(s):
	ret = []
	for c in s:
		if c in charMap:
			c = charMap[c]
		else:
			c = c.encode('gb18030')
		ret.append(c)
	return ''.join(ret)

data = []
nameStartToken = '<NaMe>'
nameEndToken   = '</nAmE>'
textStartToken = '<TeXt>'
textEndToken   = '</tExT>'
for line in open(codefile,'r').read().split('\n'):
	while nameStartToken in line:
		p = line.index(nameStartToken)
		q = line.index(nameEndToken,p)
		name = line[p+len(nameStartToken):q].decode('shiftjis')
		#if name in NameMap: name = NameMap[name]
		#line = line[:p]+convGBK(name)+line[q+len(nameEndToken):]
		line = line[:p]+name.encode("shiftjis")+line[q+len(nameEndToken):]
	while textStartToken in line:
		p = line.index(textStartToken)
		q = line.index(textEndToken,p)
		count = line[p+len(textStartToken):q]
		line = line[:p]+convGBK(Text[int(count)])+line[q+len(nameEndToken):]
	data.append(line)
open(outfile,'w').write('\n'.join(data))