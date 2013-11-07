import os,sys,struct,re
import Mjo,Util,MjsParse
import pdb
from Util import read_real_i32,read_i16
from cStringIO import StringIO as strio

bcarr = []
labels = {}
entrypoints = []
offset = 0
resstrs = {}
stack = []

def add_bc(x):
	bcarr.append(('Code', x))
	global offset
	offset += len(x)
def add_pt(x):
	global offset
	offset += 4
	bcarr.append(('Pointer', x, offset))
def add_ep(x):
	entrypoints.append(x)
def add_lb(x):
	labels[x[0]] = x[1]
def rev(l):
	ret = l[:]
	ret.reverse()
	return ret

g_output_dir = ''

def get_res(i):
	if i not in resstrs:
		print "Warning: missing resource string <%04d>\n"%i
		return "(missing string)"
	return resstrs[i]

def string_of_i16(i):
	return struct.pack('h', i)
def string_of_i32(i):
	return struct.pack('I', i&0xFFFFFFFF)
def mjstring(s):
	return string_of_i16(len(s)+1)+s+'\0'

def push(p):
	t = p.type
	if t == 'Pop':
		pass
	elif t == 'Int':
		add_bc('\0\x08' + string_of_i32(p.ctx[0]))
	elif t == 'Str':
		add_bc('\1\x08' + mjstring(p.ctx[0]))
	elif t == 'Res':
		push(('Str', get_res(p.ctx[0])))
	elif t == 'Op' and p.ctx[0] in (0x829, 0x836):
		add_bc(string_of_i16(p.ctx[0]) + string_of_i16(len(p.ctx[1])) + p.ctx[1])
	elif t == 'Op':
		add_bc(string_of_i16(p.ctx[0]) + p.ctx[1])
	elif t == 'SysCall':
		if len(p.ctx)>4:
			for i in rev(p.ctx[4].ctx): push(i)
			add_bc('\x34\x08' + string_of_i32(p.ctx[2]) + string_of_i16(len(p.ctx[4].ctx)))
		else:
			add_bc('\x34\x08' + string_of_i32(p.ctx[2]) + string_of_i16(0))
	else:
		print 'push error.'
		pdb.set_trace()

def process_cmd(stm):
	t = stm.type
	if t == 'Push':
		stack.append(stm.ctx[1])
		return
	elif t != 'Label':
		while stack:
			push(stack.pop())
	if   t == 'Entrypoint':
		add_ep((stm.ctx[1], True, offset))
	elif t == 'Function':
		add_ep((stm.ctx[1], False, offset))
	elif t == 'Label':
		s = stm.ctx[0]
		if s == '15': pdb.set_trace()
		add_lb((s, offset))
	elif t == 'Call':
		for i in rev(stm.ctx[6].ctx): push(i)
		add_bc('\x10\x08' + string_of_i32(stm.ctx[2]) + string_of_i32(stm.ctx[4]) + string_of_i16(len(stm.ctx[6].ctx)))
	elif t == 'SysCall':
		if len(stm.ctx)>4:
			for i in rev(stm.ctx[4].ctx): push(i)
			add_bc("\x35\x08" + string_of_i32(stm.ctx[2]) + string_of_i16(len(stm.ctx[4].ctx)))
		else:
			add_bc("\x35\x08" + string_of_i32(stm.ctx[2]) + string_of_i16(0))
	elif t == 'Op' and stm.ctx[0] in (0x829, 0x836):
		add_bc(string_of_i16(stm.ctx[0]) + string_of_i16(len(stm.ctx[1])) + stm.ctx[1])
	elif t == 'Op':
		if len(stm.ctx)>1: add_bc(string_of_i16(stm.ctx[0]) + stm.ctx[1])
		else: add_bc(string_of_i16(stm.ctx[0]))
	elif t == 'Exit':
		for i in rev(stm.ctx[1].ctx): push(i)
		add_bc(string_of_i16(0x82b))
	elif t == 'Jmps':
		for i in rev(stm.ctx[1].ctx): push(i)
		add_bc(string_of_i16(stm.ctx[0]))
		add_pt(stm.ctx[2])
	elif t == 'Jump':
		add_bc(string_of_i16(stm.ctx[0]))
		add_pt(stm.ctx[1])
	elif t == 'Switch':
		add_bc("\x50\x08" + string_of_i16(len(p[1])))
		for i in p[1]: add_pt(i)
	elif t == 'TextCmd':
		for i in rev(stm.ctx[1].ctx): push(i)
		add_bc('\x42\x08\x02\x00' + string_of_i16(p[1]))
	elif t == 'Text' and p[1][0] == 'Res': process_cmd(('Text', ('Str', get_res(p[1][1]))))
	elif t == 'Text' and p[1][0] == 'Str':
		tstr = "\x42\x08\x02\x00\x6e\x00"
		for s in p[1][1].split('\n'): tstr += "\x40\x08" + mjstring(s) + "\x41\x08"
		add_bc(tstr)
	else:
		print 'cmd error: '+t
		pdb.set_trace()

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

def listunifile(filename):
	f = open(filename,'rb').read()
	if f[:2]=='\xff\xfe': f = f.decode('utf16')
	elif f[:2]=='\xfe\xff': f = f[2:].decode('utf_16_be')
	elif f[:3]=='\xef\xbb\xbf': f = f[3:].decode('utf8')
	else: f = f.decode('gb18030')
	return f.replace('\r\n','\n').split(u'\n')

def LoadText(filename):
	ret = {}
	nLine = 0
	f = []
	pname = '{'
	qname = '}'
	for line in listunifile(filename):
		if pname in line:
			if qname not in line:
				pdb.set_trace()
			p = line.index(pname)
			q = line.index(qname)
			name = line[p+len(pname):q]
			if name not in NameMap:
				print 'name %s lost'%name
				continue
			line = line[:p] + NameMap[name] + line[q+len(qname):]
		f.append(line)
	while nLine<len(f):
		line = f[nLine]
		nLine += 1
		if not line or line[0] not in u'○●' or line.count(line[0])<2:
			continue
		sym=line[0]
		q=line.find(sym,1)
		if q==-1:
			print u'line: %s lost ●'%line
			continue
		try: count = int(line[1:q])
		except:
			print u'line: %s fail to get count'%line
			continue
		line = f[nLine]
		text = []
		while 1:
			if line and line[0] in u'○●': break
			text.append(line)
			nLine += 1
			if nLine>=len(f): break
			line = f[nLine]
		ret[count] = text
	return ret

def assemble(inname, outname, resname):
	#Load resources
	NameMap = LoadName()
	if os.path.isfile(resname):
		text = LoadText(resname)
	else:
		text = {}
	
	#Parse
	parser = MjsParse.Parser(open(inname, 'r').read())
	statement = parser.next()
	while statement.type != 'EOL':
		process_cmd(statement)
		statement = parser.next()
	buff = []
	for data in bcarr:
		t = data[0]
		if t == 'Code': buff.append(data[1])
		elif t == 'Pointer':
			buff.append(string_of_i32(labels[data[1]] - data[2]))
	buff = ''.join(buff)
	for id, isEntry, offset in entrypoints:
		if isEntry: entrypoint = id, offset
	
	oc = open(outname, 'wb')
	oc.write('MjPlainBytecode\0')
	oc.write(struct.pack('III', entrypoint[1], 0, len(entrypoints)))
	for id, isEntry, offset in entrypoints:
		oc.write(struct.pack('II', id, offset))
	oc.write(struct.pack('I', len(buff)))
	oc.write(buff)

def usage():
	print "Usage: mjasm [-d OUTDIR] [-o OUTNAME] [-r RESFILE] FILE"
	sys.exit()

def main(args):
	args, filelist = Util.getopts(['-d', '-o', '-r'])
	if not filelist: usage()
	inname = filelist[0]
	outname = os.path.splitext(inname)[0] + '.mjo'
	resname = os.path.splitext(inname)[0] + '.sjs'
	for s in args:
		if s == 'd': outname = args[s] + os.path.split(outname)[1]
		elif s == 'o': outname = args[s]
		elif s == 'r': resname = args[s]
		elif s in 'Hh?': usage()
		else: print 'invalid arg -%s' % s
	assemble(inname, outname, resname)

#main(['-r', r"G:\WorkPlace\想瞬譜～そうしゅんふ～\workspace\text\cgmode.txt", r'G:\WorkPlace\想瞬譜～そうしゅんふ～\workspace\code\cgmode.mjs'])
main(sys.argv[1:])
