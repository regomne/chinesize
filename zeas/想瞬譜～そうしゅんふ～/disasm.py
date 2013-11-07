'''
   Disassemble Majiro bytecode files to a Kepagoesque form.
   Copyright (c) 2005 Haeleth.

   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
   details.

   You should have received a copy of the GNU General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA  02111-1307, USA.
'''

import os,sys,struct
import Mjo,Util
from Util import read_real_i32,read_i16
from cStringIO import StringIO as strio

g_output_dir = ''
include_offsets = 'None'
include_markers = False
separate_strings = 'Yes'

class Cmd:
	def __init__(self, s, i, a):
		self._ctx = (s, i, a)
		self.__getitem__ = self._ctx.__getitem__
		self.__len__ = self._ctx.__len__
		self.__getslice__ = self._ctx.__getslice__
		self.__eq__ = self._ctx.__eq__

class Stack:
	def __init__(self, s, i, a):
		self._ctx = (s, i, a)
		self.__getitem__ = self._ctx.__getitem__
		self.__len__ = self._ctx.__len__
		self.__getslice__ = self._ctx.__getslice__
		self.__eq__ = self._ctx.__eq__

class resstrs:
	def __init__(self, fname):
		self.enabled = True
		self._fname = ''
		self._buf = b''
		self._res = []
		self._count = 0
		self._fname = fname
		if   separate_strings == 'Yes':
			#Object to handle resource string output
			self._fname = fname
		elif separate_strings == 'Suppress':
			#Stub object for when we don't want to clobber the resource file
			pass
		elif separate_strings == 'No':
			#Stub object for when resstrs aren't wanted at all
			self.enabled = False
			self.add_whole = lambda self, s: ''
			self.add_part = lambda self, s: ''
		else:
			raise Exception('Unknown separate type ' + separate_strings)
	
	def _write(self, s):
		pre, post = "", ""
		if s:
			if s[0] == ' ':
				pre = '\\'
			if s[-1] == ' ':
				post = '\\'
		if self._count - 1 != len(self._res):
			raise Exception('Resource count unmatch')
		s = (pre + s + post).decode('shiftjis')
		if '{' in s or '}' in s:
			import pdb
			pdb.set_trace()
		self._res.append(s)
	
	def _last(self): return '#res<%04d>'%self._count
	
	def add_whole(self, s):
		self._count += 1
		self._write(s)
		return self._last()
	
	def add_part(self, s):
		if len(self._buf) == 0:
			self._count += 1
			self._buf += s
			return self._last()
		else:
			self._buf += s
			return ''
	
	def finalise(self):
		if self._buf:
			self._write(self._buf)
		self._buf = b''
	
	def _save(self):
		namepath = sys.path[0]+'\\name.txt'
		if os.path.isfile(namepath):
			name = open(namepath,'rb').read().decode('utf16').split()
		else:
			name = []
		newname = []
		nglist = []
		ignoreChar = [c for c in u'　…“”《》：；、。（）―—！？「」『』【】']
		ignoreChar +=[unichr(c) for c in range(32,127)+range(0xff01,0xff5f)]
		text = []
		for n in range(len(self._res)):
			line = self._res[n]
			count = n + 1
			isname = line in name
			if not isname and len(line)<10:
				#isname = True
				for c in u'―「」。…！？n':
					if c in line:
						isname = False
						break
				if isname:
					for c in ignoreChar:
						if c in line:
							if line not in nglist:
								nglist.append(line)
							isname = False
							break
			if isname:
				if line not in name+newname:
					newname.append(line)
				text.append(u'○%04d○'%count + '{'+line+'}')
			else:
				text.append(u'○%04d○'%count + line)
				disp = []
				p = 0
				while p<len(line) and line[p] in ignoreChar:
					disp.append(line[p])
					p += 1
				line = line[p:]
				p = len(line)-1
				suffix = []
				while p>0 and line[p] in ignoreChar:
					suffix.append(line[p])
					p -= 1
				suffix.reverse()
				text.append(u'●%04d●'%count + ''.join(disp+suffix))
				text.append(u';'+u'━'*32)
		open(self._fname,'wb').write('\r\n'.join(text).encode('utf16'))
		if nglist:
			print 'not a name:'
			print '\n'.join(nglist).encode('gb18030')
			os.system('pause')
		if newname:
			print 'newname:'
			print '\n'.join(newname).encode('gb18030')
			os.system('pause')
		name.sort()
		open(sys.path[0]+'\\newname.txt','wb').write('\r\n'.join(name).encode("utf16"))
	
	def close(self):
		self.finalise()
		if self.enabled and len(self._res):
			self._save()

def read_bytes(ic, n): return '#'+ic.read(n).encode('hex')

def read_mjstring(ic):
	length = struct.unpack('H', ic.read(2))[0]
	dat = ic.read(length)
	assert(length == len(dat))
	return dat.strip('\0')

def contains_sjs(s):
	for c in s:
		if ord(c)>0x80:
			return True
	return False

#return may be: None, string, Stack(str, int, bool), Cmd(str, int, list)
def read_cmd(ic, startpos):
	cmd    = lambda    s: Cmd  (s, 0, [])
	cmdi   = lambda i, s: Cmd  (s, i, [])
	stack  = lambda    s: Stack(s, 0, False)
	stacki = lambda i, s: Stack(s, i, False)
	read_ptr = lambda : read_real_i32(ic) + ic.tell()
	op = struct.unpack('H', ic.read(2))[0]
	if   op in (0x100, 0x101, 0x108, 0x109, 0x110, 0x118, 0x119, 0x11a, 0x120,
	             0x121, 0x130, 0x138, 0x140, 0x142, 0x148, 0x150, 0x158, 0x15a,
	             0x160, 0x162, 0x170, 0x178, 0x180, 0x188, 0x190, 0x1a0, 0x1a1):
		return cmd('op%03x'%op)
	elif op in (0x1b0, 0x1b1, 0x1b2, 0x1b3, 0x1b5, 0x1b8, 0x1c0, 0x1d0, 0x1d2,
	             0x1d8, 0x1d9, 0x270, 0x272, 0x290, 0x2c0):
		return cmd('op%03x[%s]'%(op, read_bytes(ic, 8)))
	elif op in (0x800,): return stack('%ld'%read_real_i32(ic))
	elif op in (0x801,):
		s = read_mjstring(ic)
		if contains_sjs(s): return Stack(s, 0, True)
		else:               return stack(repr(s))
	elif op in (0x802,): return stack("op802[%s]"%read_bytes(ic,8))
	elif op in (0x803,): return cmd("op803[%s]"%read_bytes(ic,4))
	elif op in (0x80f,): return cmd("op80f[%s]"%read_bytes(ic,10))
	elif op in (0x810,):
		a,b = struct.unpack('ii', ic.read(8))
		argc = read_i16(ic)
		return cmdi(argc, "call<$%08lx, %ld>"%(a&0xFFFFFFFF,b))
	elif op in (0x829,):
		cnt, s = read_i16(ic), ''
		if cnt: s = read_bytes(ic,cnt)
		return cmd("op829[%s]"%s)
	elif op in (0x82b,): return cmdi(1,'exit')
	elif op in (0x82c,): return Cmd('goto', 0, [read_ptr()])
	elif op in (0x82d,): return Cmd('jmp82d', 0, [read_ptr()])
	elif op in (0x82e,): return Cmd('jmp82e', 0, [read_ptr()])
	elif op in (0x82f,): return cmd('op82f')
	elif op in (0x830,): return cmd('op830[%s]'%read_bytes(ic,4))
	elif op in (0x831,): return Cmd('jne',1,[read_ptr()])
	elif op in (0x832,): return Cmd('jmp832',1,[read_ptr()])
	elif op in (0x834,):
		a = read_real_i32(ic)
		argc = read_i16(ic)
		return stacki(argc, 'syscall<$%08lx>'%(a&0xFFFFFFFF))
	elif op in (0x835,):
		a = read_real_i32(ic)
		argc = read_i16(ic)
		return cmdi(argc, 'syscall<$%08lx>'%(a&0xFFFFFFFF))
	elif op in (0x836,):
		cnt, s = read_i16(ic), ''
		if cnt: s = read_bytes(ic,cnt)
		return cmd("op836[%s]"%s)
	elif op in (0x837,): return cmd('op837[%s]'%read_bytes(ic,8))
	elif op in (0x838,): return Cmd("jmp838", 1, [read_ptr ()])
	elif op in (0x839,): return Cmd("jmp839", 1, [read_ptr ()])
	elif op in (0x83a,):
		n = read_i16(ic)
		if not include_markers: return None
		else: return cmd("{-line %d-}" % n)
	elif op in (0x83b,): return Cmd("jmp83b", 0, [read_ptr ()])
	elif op in (0x83d,): return Cmd("jmp83d", 0, [read_ptr ()])
	elif op in (0x83e,): return cmd("op83e")
	elif op in (0x83f,): return cmd('op83f')
	elif op in (0x840,): return read_mjstring(ic)
	elif op in (0x841,):
		if not include_markers: return None
		else: return cmd('op841')
	elif op in (0x842,):
		sc = read_i16(ic)
		if sc != 2:
			raise Exception('unknown subcode %d to op842 at 0x%06x'%(sc, startpos))
		op = read_i16(ic)
		if   op == 0x66: return cmdi(5, "text_control_66")
		elif op == 0x6e:
			if separate_strings: return '\\n'
			else: return cmd('br')
		elif op == 0x6f: return cmd('text_control_6f')
		elif op == 0x70: return cmd('pause')
		elif op == 0x72: return cmd('text_control_72')
		elif op == 0x73: return cmdi(1, 'text_control_73')
		elif op == 0x77: return cmd('cls')
		else: raise Exception('unknown command text_control_%02x at 0x%06x'%(op, startpos))
	elif op in (0x843,): return Cmd('jmp843', 0, [read_ptr()])
	elif op in (0x844,): return cmd('op844') #target for jmp83b, jmp83d?
	elif op in (0x847,): return cmd('op837[%s]'%read_bytes(ic,4))
	elif op in (0x850,):
		c = read_i16(ic)
		return Cmd('switch', 0, [read_ptr() for n in range(c)])
	else: raise Exception('unknown command op%03x at 0x%06x'%(op, startpos))

def process(fname):
	ic = open(fname,'rb')
	encrypted, d, offset = Mjo.header(ic)

	#Get entrypoint details
	ic.seek(0x10)
	primary = Util.read_int(ic)
	ic.seek(0x18)
	record_count = Util.read_int(ic)
	entrypoints = {}
	for i in range(record_count):
		code = Util.read_int32(ic)
		offs = Util.read_int(ic)
		entrypoints[offs] = (code, offs == primary)

	#Get generic channel for bytecode
	if encrypted:
		s = Mjo.crypt(ic)
		bc = strio(s)
		ic.seek(0)
		open(fname+'.dec','wb').write(ic.read(offset) + s)
	else:
		ic.seek(offset)
		s = ic.read()
		bc = strio(s)
	endPos = len(s) - 1

	#Read commands
	cmds = []
	targets = {}
#	try:
	if 1:
		while bc.tell()<endPos:
			cpos = bc.tell()
			cmd = read_cmd(bc, cpos + offset)
			cmds.append((cpos, cmd))
			if cmd.__class__ == Cmd:
				for i in cmd[2]:
					targets[i] = 0
#	except:
	if 0:
		print 'warning: this mjo may be truncated.'
		import pdb
		pdb.set_trace()
	
	#Order labels
	targets = targets.keys()
	targets.sort(reverse=True)
	labels = {}
	for n in range(len(targets)):
		labels[targets[n]] = n + 1
	
	#Open output file
	dir, basename = os.path.split(fname)
	output_dir = g_output_dir
	if not output_dir:
		output_dir = dir
	noext = os.path.splitext(basename)[0]
	oc = open(output_dir + '\\' + noext + '.mjs', 'w')
	res = resstrs(output_dir + '\\' + noext + '.sjs')
	
	#Output code
	def do_lbl(pos):
		if pos in labels:
			oc.write('\n  @%d\n'%labels[pos])
			res.finalise()
			if pos in targets:
				targets.remove(pos)
	
	def do_pos(pos):
		if include_offsets == 'Bytecode': oc.write("{-%04x-} "%pos)
		elif include_offsets == 'File': oc.write("{-%04x-} "%(pos+offset))
	
	stack = []
	def pop_all():
		if stack:
			res.finalise()
			while stack:
				pos, elt = stack.pop()
				do_lbl(pos)
				do_pos(pos)
				oc.write('push %s\n'%elt)
	
	def pop_n(n, clear = True):
		skip = None
		l = []
		for n in range(n):
			if skip or not stack:
				l.append('pop')
			else:
				pos, elt = stack.pop()
				if not skip and pos in labels:
					skip = pos
				l.append(elt)
		if clear or skip:
			pop_all()
			if skip: do_lbl(skip)
		return ', '.join(l)
	
	for pos, cmd in cmds:
		if pos in entrypoints:
			code, is_entry = entrypoints[pos]
			del entrypoints[pos]
			f = "function"
			if is_entry: f = "entrypoint"
			oc.write("\n#%s $%08lx\n"%(f, code&0xFFFFFFFF))
			res.finalise()
		if cmd == None:
			pop_all()
			do_lbl(pos)
		elif cmd.__class__ == Stack:
			s, n, b = cmd
			res.finalise()
			if n: s = "%s(%s)"%(s,pop_n(n, False))
			elif b:
				if res.enabled: s=res.add_whole(s)
				else: s = repr(s)
			stack.append((pos,s))
		elif cmd.__class__ == Cmd:
			s, n, l = cmd
			if n: args = " (%s)"%pop_n(n)
			else:
				args = ""
				pop_all()
			do_lbl(pos)
			do_pos(pos)
			res.finalise()
			oc.write(s)
			oc.write(args)
			if len(l) == 1:
				oc.write(' @%d'%labels[l[0]])
			elif l:
				oc.write(' { %s }'%', '.join(['@%d'%labels[n] for n in l]))
			oc.write('\n')
		else:#text
			pop_all()
			do_lbl(pos)
			do_pos(pos)
			if res.enabled:
				r = res.add_part(cmd)
				if r: oc.write("%s\n"%r)
			else: oc.write(repr(cmd))
	pop_all()
	
	#Clean up
	if targets:
		print "Warning: labels not generated: %s" % ", ".join(['@%d'%labels[n] for n in targets])
	if entrypoints:
		print "Warning: entrypoints not generated: %s"%", ".join(['$%08lx'%entrypoints[n][0]&0xFFFFFFFF for n in entrypoints])
	oc.close()
	res.close()

def usage():
	print "Usage: mjdisasm [-hlLms] [-d OUTDIR] FILE"
	sys.exit()

def opts(args):
	global include_offsets, include_markers, g_output_dir, separate_strings
	for s in args:
		if s in 'Hh?': usage()
		elif s == 'd': g_output_dir = args[s]
		elif s == 'l': include_offsets = 'Bytecode'
		elif s == 'L': include_offsets = 'File'
		elif s == 'm': include_markers = True
		elif s == 's': separate_strings = 'No'
		elif s == 'c': separate_strings = 'Suppress'
		else: print 'invalid arg -%s' % s

def main():
	args, filelist = Util.getopts(['d'])
	opts(args)
	if not filelist: usage()
	for filename in filelist:
		process(filename)

main()