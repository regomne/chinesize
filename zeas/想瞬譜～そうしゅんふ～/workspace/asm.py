import os,sys,struct,ctypes
#import Mjo
from cStringIO import StringIO as strio

normal_encoding = 'gbk'
#normal_encoding = 'shiftjis'

def listunifile(filename):
	f = open(filename,'rb').read()
	if f[:2]=='\xff\xfe': f = f.decode('utf16')
	elif f[:2]=='\xfe\xff': f = f[2:].decode('utf_16_be')
	elif f[:3]=='\xef\xbb\xbf': f = f[3:].decode('utf8')
	else: f = f.decode('gb18030')
	return f.replace('\r\n','\n').split(u'\n')

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

ignoreChar = [c for c in u'　…“”《》：；、。（）―—！？「」『』【】']
ignoreChar +=[unichr(c) for c in range(32,127)+range(0xff01,0xff5f)]
charmap = {
	u'\u266a': u'\u222e',
	u'\u301c': u'·',
}

#S = u16 slen, s8[slen]; i = -123; x = 0xfe1; 1~N = #fea9b4; l = label; h = u16
opInfo = {
	0x800: ('int', 'i'),
	0x801: ('str', 't'),
	0x80f: ('op80f', 'xlh'),
	0x810: ('syscall', 'xih'),
	0x829: ('op829', 'X'),
	0x82b: ('return', []),
	0x82c: ('goto', 'l'),
	0x82d: ('jz', 'l'),
	0x82e: ('jnz', 'l'),
	0x82f: ('pop', []),
	0x830: ('j830', 'l'),
	0x831: ('jne', 'l'),
	0x832: ('jle', 'l'),
	0x833: ('jge', 'l'),
	0x834: ('call', 'xh'),
	0x835: ('stdcall', 'xh'),
	0x836: ('op836', 'X'),
	0x838: ('jg', 'l'),
	0x839: ('jl', 'l'),
	0x83a: ('line', 'h'),
	0x83b: ('jmp83b', 'l'),
	0x83d: ('jmp83d', 'l'),
	0x840: ('strcpy', 't'),
	0x841: ('preptext', []),
	0x842: ('textcontrol', 'hh'),
	0x843: ('jmp843', 'l'),
	0x850: ('switch', 'L'),
}

for op in (0x100, 0x101, 0x108, 0x109, 0x110, 0x118, 0x119, 0x11a, 0x120,
			0x121, 0x130, 0x138, 0x140, 0x142, 0x148, 0x150, 0x158, 0x15a,
			0x160, 0x162, 0x170, 0x178, 0x180, 0x188, 0x190, 0x1a0, 0x1a1,
			0x83e, 0x83f, 0x841, 0x844, ):
	opInfo[op] = ('op%03x'%op, [])
for op in (0x803, 0x847, ):
	opInfo[op] = ('op%03x'%op, [4])
for op in (0x1b0, 0x1b1, 0x1b2, 0x1b3, 0x1b5, 0x1b8, 0x1c0, 0x1d0, 0x1d2,
	        0x1d8, 0x1d9, 0x270, 0x272, 0x290, 0x2c0, 0x802, 0x837):
	opInfo[op] = ('op%03x'%op, [8])

class Reader():
	def __init__(self, filename):
		self.buff = open(filename, 'rb').read().decode("utf16").replace('\r\n','\n')
		self.p = 0
	
	def read(self, bytes = 1):
		if self.p < len(self.buff):
			ret = self.buff[self.p:self.p+bytes]
			self.p += len(ret)
			return ret
		return u''
	
	def peek(self, bytes = 1): return self.buff[self.p:self.p+bytes]
	def tell(self): return self.p
	def seek(self, pos): self.p = pos
	def readline(self):
		ret = []
		while 1:
			c = self.read()
			ret.append(c)
			if not c or c in u'\r\n':
				break
		return u''.join(ret)

class Assembler():
	def __init__(self, codefile, resfile = ''):
		self.codebuff = []
		self.codeoffset = 0
		self.labels = {}
		self.entrypoints = []
		self.rd = Reader(codefile)
		self.nameMap = LoadName()
		self.opInfo = {}
		self.res = {}
		if resfile:
			self.LoadText(resfile)
		for op in opInfo:
			opname, arg = opInfo[op]
			self.opInfo[opname] = (op, arg)
	
	def LoadText(self, filename):
		Text = {}
		for line in listunifile(filename):
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
					if not line.strip(): continue
					while p<len(line) and line[p] in ignoreChar: p+=1
					if not line[p:] and line[:p] == Text[n][:p]: continue
				#补全括号
				if origline[0] == u'●':
					origline=Text[n]
					if origline and origline[-1] in ignoreChar:
						p=unichr(ord(origline[-1])-1)
						if line.count(p)>line.count(origline[-1]):
							line+=origline[-1]
				#替换人名
				while '{' in line and '}' in line:
					p = line.index('{')
					q = line.index('}')
					name = line[p+1:q]
					if name in self.nameMap: name = self.nameMap[name]
					line = line[:p] + name + line[q+1:]
				t = []
				for c in line:
					if c in charmap:
						c = charmap[c]
					t.append(c)
				Text[n] = ''.join(t)
		self.res = Text
	
	def splitLine(self, line):
		ret = []
		p = 0
		l = []
		while p < len(line):
			if line[p] in ', \t':
				ret.append(''.join(l))
				l = []
				while p < len(line) and line[p] in ', \t': p += 1
			elif line[p] == '"':
				l.append('"')
				p += 1
				while line[p]!='"':
					if line[p] == '\\':
						p += 1
						l.append(eval('"\\'+line[p]+'"'))
					else:
						l.append(line[p])
					p += 1
				l.append('"')
				p += 1
			else:
				l.append(line[p])
				p += 1
		if l: ret.append(''.join(l))
		return ret
	
	def asm(self):
		self.tmpbuff = []
		self.codelen = 0
		def WORD(i): return struct.pack('H', i)
		def addcode(data):
			self.tmpbuff.append(('code', data))
			self.codelen += len(data)
		def addlabel(label):
			self.tmpbuff.append(('label', label))
			self.codelen += 4
		def applycode():
			self.codeoffset += self.codelen
			for t, data in self.tmpbuff:
				if t == 'label':
					data = (data, self.codeoffset)
				self.codebuff.append((t, data))
			self.codelen = 0
			self.tmpbuff = []
		while 1:
			line = self.rd.readline()
			if not line: break
			line = line.strip()
			if not line: continue
			if line[0] == '#':#entrypoint
				entrytype, hash = line[1:].split()
				self.entrypoints.append((entrytype, eval(hash), self.codeoffset))
				continue
			elif line[0] == ';':#comment
				continue
			elif line[0] == '@':#label
				self.labels[line[1:]] = self.codeoffset
				continue
			line = self.splitLine(line)
			op = line[0]
			args = line[1:]
			args.reverse()
			if op not in self.opInfo:
				print 'unknown op %s'%op
				pdb.set_trace()
			op, arginfo = self.opInfo[op]
			addcode(WORD(op))
			for t in arginfo:
				if t == 'L':
					addcode(WORD(len(args)))
					applycode()
					while args:
						arg = args.pop()
						if arg[0]!='@':
							print 'unexpected label'
							pdb.set_trace()
						self.codeoffset += 4
						self.codebuff.append(('label', (arg[1:], self.codeoffset)))
					continue
				arg = args.pop()
				assert(arg)
				labelbuff = []
				if type(t) == int:
					if arg[0]!='#':
						print 'unexpected hex string'
						pdb.set_trace()
					addcode(arg[1:].decode('hex'))
				elif t == 'l':
					if arg[0]!='@':
						print 'unexpected label'
						pdb.set_trace()
					addlabel(arg[1:])
				elif t == 'i':
					addcode(struct.pack('i', eval(arg)))
				elif t == 'x':
					addcode(struct.pack('I', eval(arg)))
				elif t == 'X':
					if arg[0]!='#':
						print 'unexpected hex string'
						pdb.set_trace()
					arg = arg[1:].decode('hex')
					addcode(WORD(len(arg))+arg)
				elif t == 'h':
					addcode(WORD(eval(arg)))
				elif t in 'st':
					if arg[0] == '"':
						arg = arg[1:-1].encode(normal_encoding)+'\0'
					elif arg.startswith('res<'):
						arg = self.res[int(arg[arg.index('<')+1:arg.index('>')])].encode(normal_encoding)+'\0'
					else:
						print 'unexpected string'
						pdb.set_trace()
					addcode(WORD(len(arg)) + arg)
			applycode()
		buff = []
		for t, b in self.codebuff:
			if t == 'code':
				buff.append(b)
			elif t == 'label':
				label, offset = b
				buff.append(struct.pack('i', self.labels[label] - offset))
		codebuff = ''.join(buff)
		for t, hash, offset in self.entrypoints:
			if t == 'entry':
				primaryentry = offset
				break
		buff = ['MajiroObjV1.000\0']
		buff.append(struct.pack('III', primaryentry, 0, len(self.entrypoints)))
		for t, hash, offset in self.entrypoints:
			buff.append(struct.pack('II', hash, offset))
		buff.append(struct.pack('I', len(codebuff)))
		return ''.join(buff)+codebuff
opts = {'codefile':sys.argv[1]}
def setter(name, val):
	opts[name] = val
optinf = {
	'-codefile': lambda v: setter('codefile', v),
	'-resfile': lambda v: setter('resfile', v),
	'-o': lambda v: setter('outname', v),
}

for opt in optinf:
	if opt in sys.argv:
		n = sys.argv.index(opt)
		optinf[opt](sys.argv[n + 1])

if 'resfile' in opts:
	open(opts['outname'],'wb').write(Assembler(opts['codefile'], opts['resfile']).asm())
else:
	open(opts['outname'],'wb').write(Assembler(opts['codefile']).asm())
print os.path.split(opts['outname'])[1] + ' created.'