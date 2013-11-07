#!/usr/bin/env python3

# BGI ._bp file disassembler

import glob
import os
import struct
import sys

import bpop

def get_code_end(data):
	pos = -1
	while 1:
		res = data.find(b'\x17', pos+1)
		if res == -1:
			break
		pos = res
	return pos+1

def parse(code):
	bpop.clear_offsets()
	inst = {}
	size = get_code_end(code)
	pos = 0
	while pos < size:
		addr = pos
		op = code[addr]
		if op not in bpop.ops:
			raise Exception('size unknown for op %02x @ offset %05x' % (op, addr))
		pos += 1
		fmt, pfmt, fcn = bpop.ops[op]
		if fmt:
			n = struct.calcsize(fmt)
			args = struct.unpack(fmt, code[pos:pos+n])
			if fcn:
				args = fcn(code, addr, *args)
			inst[addr] = pfmt % args
			pos += n
		else:
			inst[addr] = pfmt
	offsets = bpop.offsets.copy()
	return inst, offsets
	
def out(fo, inst, offsets):
	for addr in sorted(inst):
		if addr in offsets:
			fo.write('\nL%05x:\n' % addr)
		fo.write('\t%s;\n' % inst[addr])
		
def dis(file):
	ofile = os.path.splitext(file)[0] + '.bpd'
	fi = open(file, 'rb')
	hdrsize, = struct.unpack('<I', fi.read(4))
	fi.seek(hdrsize, 0)
	code = fi.read()
	fi.close()
	inst, offsets = parse(code)
	fo = open(ofile, 'w', encoding='utf-8')
	out(fo, inst, offsets)
	fo.close()
	
if __name__ == '__main__':
	if len(sys.argv) < 2:
		print('Usage: bpdis.py <file(s)>')
		print('(only ._bp files amongst <file(s)> will be processed)')
		sys.exit(1)
	for arg in sys.argv[1:]:
		for script in glob.glob(arg):
			base, ext = os.path.splitext(script)
			if ext == '._bp':
				print('Disassembling %s...' % script)
				dis(script)
	