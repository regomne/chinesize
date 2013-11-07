import os,sys,struct,ctypes,pdb
import Mjo
from cStringIO import StringIO as strio

normal_encoding = 'shiftjis'
#normal_encoding = 'gbk'

splitext = 1
text = []
textcount = 1

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

sys.argv=win32_unicode_argv()

def listunifile(filename):
	f = open(filename,'rb').read()
	if f[:2]=='\xff\xfe': f = f.decode('utf16')
	elif f[:2]=='\xfe\xff': f = f[2:].decode('utf_16_be')
	elif f[:3]=='\xef\xbb\xbf': f = f[3:].decode('utf8')
	else: f = f.decode('gb18030')
	return f.replace('\r\n','\n').split(u'\n')

def LoadName():
	NameMap={}
	namelist=[]
	namefile = sys.path[0]+'\\name.txt'
	if os.path.isfile(namefile):
		for name in listunifile(namefile):
			NameMap[name] = 0
	return NameMap

namelist = LoadName()

def isText(s):
	for c in s:
		if ord(c)>0x80: return True
	return False


ignoreChar = [c for c in u'　…“”《》：；、。（）―—！？「」『』【】']
ignoreChar +=[unichr(c) for c in range(32,127)+range(0xff01,0xff5f)]
escchrmap = {
	'\r' : '\\r',
	'\n' : '\\n',
	'"' : '\\"',
	'\'' : '\\\'',
	'\\' : '\\\\',
}

def WORD(s): return struct.unpack('H', s.read(2))[0]
def GetStr(s):
	dat = s.read(WORD(s)).strip('\0').decode(normal_encoding)
	line = []
	for c in dat:
		if c in escchrmap:
			c = escchrmap[c]
		line.append(c)
	return ''.join(line)
	
def GetText(s, collectText = True):
	line = GetStr(s)
	if isText(line):
		global textcount
		if line in namelist:
			text.append(u'○%04d○'%textcount + '{'+line+'}')
		else:
			text.append(u'○%04d○'%textcount + line)
			disp = []
			p=0
			while p<len(line) and line[p] in ignoreChar:
				disp.append(line[p])
				p+=1
			line = line[p:]
			p = len(line)-1
			suffix = []
			while p>0 and line[p] in ignoreChar:
				suffix.append(line[p])
				p -= 1
			suffix.reverse()
			text.append(u'●%04d●'%textcount + ''.join(disp+suffix))
			text.append(u';'+u'━'*32)
		ret = "res<%04d>"%textcount
		textcount += 1
		return ret
	else:
		return '"' + line + '"'

def textctrl(s):
	origoff = s.tell()
	subop, cmd = struct.unpack('HH', s.read(4))
	if cmd == 0x6e:
		global textcount
		text.append(u'○%04d○\\n'%textcount)
		text.append(u'●%04d●\\n'%textcount)
		text.append(u';'+u'━'*32)
		textcount += 1
	s.seek(origoff)
	return 'hh'

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
	0x83a: (';line', 'h'),
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

ic = open(sys.argv[1],'rb')
encrypted, l, offset = Mjo.header(ic)
ic.seek(16)
primary = struct.unpack('I', ic.read(4))[0]
ic.seek(0x18)
entrypoints = {}
for i in range(struct.unpack('I', ic.read(4))[0]):
	code, offs = struct.unpack('II', ic.read(8))
	entrypoints[offs] = (code, offs == primary)
if encrypted:
	s = strio(Mjo.crypt(ic))
else:
	ic.seek(offset)
	s = strio(ic.read(l))

dasm = []
labels={}
ident = 0
while 1:
	curoff = s.tell()
	if curoff in entrypoints:
		code, isEntry = entrypoints[curoff]
		if isEntry: dasm.append((curoff, '#entry 0x%08x'%code))
		else:    dasm.append((curoff, '#function 0x%08x'%code))
	
	op = s.read(2)
	if not op: break
	op = struct.unpack('H', op)[0]
	if op in opInfo:
		op, f = opInfo[op]
		args = []
		for t in f:
			if type(t) == int:
				args.append('#'+s.read(t).encode("hex"))
			elif t == 'l':
				#lblname = '@loc_%d'%len(labels)
				#labels[offset] = lblname
				args.append(struct.unpack('i',s.read(4))[0])
			elif t == 'L':
				for n in range(WORD(s)):
					offset = struct.unpack('i',s.read(4))[0] + s.tell()
					if offset in labels:
						lblname = labels[offset]
					else:
						lblname = '@loc_%d'%len(labels)
						labels[offset] = lblname
					args.append(lblname)
			elif t=='i':
				args.append(str(struct.unpack('i', s.read(4))[0]))
			elif t=='x':
				args.append('0x%X'%struct.unpack('I', s.read(4))[0])
			elif t=='X':
				args.append('#'+s.read(WORD(s)).encode("hex"))
			elif t=='h':
				args.append(str(WORD(s)))
			elif t=='s':
				args.append('"' + GetStr(s) + '"')
			elif t=='t':
				args.append(GetText(s))
			else:
				print 'unknown arg: ' + str(t)
				pdb.set_trace()
		for n in range(len(args)):
			if type(args[n]) in (int, long):
				offset = args[n] + s.tell()
				if offset in labels:
					lblname = labels[offset]
				else:
					lblname = '@loc_%d'%len(labels)
					labels[offset] = lblname
				args[n] = lblname
		if args:
			op += ' ' + ', '.join(args)
		dasm.append((curoff, '\t'*ident + op))
	else:
		print '\n'.join([n[1] for n in dasm[-10:]])
		print 'unknown op %03d @ %Xh'%(op, curoff)
		pdb.set_trace()
		op = 'op%03d' % op
		dasm.append('\t'*ident + op + ' ' + ', '.join([GetVal(s) for n in range(WORD(s))]))

code = []
for offset, line in dasm:
	if offset in labels:
		code.append('')
		code.append(labels[offset])
	code.append(line)

open(sys.argv[1]+'.code','wb').write('\r\n'.join(code).encode('utf16'))
if text: open(sys.argv[1]+'.txt','wb').write('\r\n'.join(text).encode('utf16'))
#if namechk: os.system('pause')
