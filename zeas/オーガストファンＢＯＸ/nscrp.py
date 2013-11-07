import os,sys

keepcomment = 1

def isLabelText(s):
	for f in ['caption "','insertmenu "','mov $name2,"','mov $129,"','add $10,"','mov $name  ,"','mov $save_title,"',
				'mov $sel_text1,"','mov $sel_text2,"','mov $sel_text3,"','mov $quiz_text,"'
			]:
		if s.startswith(f):
			return True
	for f in [' add $102,"']:
		if f in s:
			return True
	return False

def isText(s):
	for p in range(len(s)):
		if ord(s[p])>0x2000:
			return (p,)
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
    u'【': u'【',
    u'】': u'】',
    u'／': u'／',
    u'※': u'※',
}
suffixIgnore = {
	u' ' : u' ',
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
prefixIgnore.update(suffixIgnore)
for n in range(10):
	n = unicode(n)
	prefixIgnore[n]=n

def SetName(p,q,line):
	global code, nameList
	name = line[p:q]
	code.append(line[:p]+'<NaMe>'+name+'</nAmE>'+line[q:])
	if name not in nameList: nameList.append(name)

def getDispLine(line):
	displine=[]
	for c in line:
		if c in prefixIgnore:
			displine.append(prefixIgnore[c])
		elif ord(c)<128:
			displine.append(c)
		else: break
	return ''.join(displine)

def removeComment(s):
	r =[s[0]]
	for line in s[1:]:
		p = 0
		while p<len(line):
			if line[p] == '"':
				p+=1
				while p<len(line) and line[p]!='"':
					p+=1
			if p>=len(line) or line[p] == ';':
				break
			p+=1
		r.append(line[:p].rstrip(' \t:'))
	return r

code=[]
text=[]
nameList=[]
counter= 0
source = removeComment(open(sys.argv[1],'r').read().decode('932').split('\n'))
nLine = 0
while nLine < len(source):
	line = source[nLine]
	if not line or line[0] in ';*':
		name = ''
		if not line or line[0] != ';': code.append(line)
		elif keepcomment: code.append(line)
		nLine += 1
		continue
	if line.strip().startswith('mov $name  ,"') and isText(line):
		if line.count('"')!=2:
			import pdb
			pdb.set_trace()
		q = line.rindex('"')
		p = line[:q].rindex('"')+1
		name = line[p:q]
		SetName(p,q,line)
	elif isText(line) and isLabelText(line):
		q = line.rindex('"')
		p = line.index('"')+1
		while '"' in line[p:q]:
			r = line[:q].rindex('"')
			if isText(line[r+1:q]):
				break
			else:
				q = r
		counter += 1
		code.append(line[:p]+'<TeXt>%d</tExT>'%counter+line[q:])
		s = u'○○%d○;'%(counter)
		text.append(s + u'-'*(64 - len(s.encode('gb18030'))))
		text.append(line[p:q])
		s = u'●●%d●;'%(counter)
		text.append(s + u'-'*(64 - len(s.encode('gb18030'))))
		text.append(getDispLine(line[p:q]))
	elif line.startswith('csel'):
		if '"' in line:
			import pdb
			pdb.set_trace()
		code.append(line)
		line = source[nLine+1]
		while '"' in line:
			q = line.rindex('"')
			p = line[:q].rindex('"')+1
			counter += 1
			code.append(line[:p]+'<TeXt>%d</tExT>'%counter+line[q:])
			s = u'○选择○%d○;'%(counter)
			text.append(s + u'-'*(64 - len(s.encode('gb18030'))))
			text.append(line[p:q])
			s = u'●选择●%d●;'%(counter)
			text.append(s + u'-'*(64 - len(s.encode('gb18030'))))
			text.append(getDispLine(line[p:q]))
			nLine+=1
			line = source[nLine+1]
	elif line[0]=='#' or ord(line[0])>0x2000 or 47<ord(line[0])<58:#Text&num
		if not isText(line):
			print u'无文本: '+line
			code.append(line)
			nLine += 1
			continue
		counter += 1
		s = u'○%s○%d○;'%(name, counter)
		text.append(s + u'-'*(64 - len(s.encode('gb18030'))))
		disp = []
		while isText(line) and (line[0]=='#' or ord(line[0])>0x2000 or 47<ord(line[0])<58):
			displine = []
			if not isText(line):
				print u'无文本: '+line
			for c in line:
				if c not in prefixIgnore: break
				displine.append(prefixIgnore[c])
			disp.append(u''.join(displine))
			text.append(line)
			nLine += 1
			if len(disp)>10 and '\\' in line: break
			line = source[nLine]
		s = u'●%s●%d●;'%(name, counter)
		text.append(s + u'-'*(64 - len(s.encode('gb18030'))))
		text.append('\r\n'.join(disp))
		code.append('<TeXt>%d</tExT>'%counter)
		print counter, '\r',
		continue
	else:
		if isText(line):
			if line.count('"'):
				p = line.index('"')+1
				q = line.index('"',p)
				while '"' in line[p:q]:
					r = line[:q].rindex('"')
					if isText(line[r+1:q]):
						break
					else:
						q = r
				counter += 1
				code.append(line[:p]+'<TeXt>%d</tExT>'%counter+line[q:])
				s = u'○○%d○;'%counter
				text.append(s + u'-'*(64 - len(s.encode('gb18030'))))
				text.append(line[p:q])
				s = u'●●%d●;'%counter
				text.append(s + u'-'*(64 - len(s.encode('gb18030'))))
				text.append(getDispLine(line[p:q]))
				print '\t'+line.encode('gb18030')
			else:
				code.append(line)
				print line
		else: code.append(line)
	nLine+=1

nameList.sort()
open(sys.argv[1]+'.txt','wb').write('\r\n'.join(text).encode('utf16'))
open(sys.argv[1]+'.code','wb').write('\r\n'.join(code).encode('utf16'))
open(sys.path[0]+'\\name.txt','wb').write(u'\r\n'.join(nameList).encode("utf16"))