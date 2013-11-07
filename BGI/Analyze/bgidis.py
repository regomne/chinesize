#!/usr/bin/env python3

# BGI script file disassembler

import glob
import os
import struct
import sys
import base64

import asdis
import bgiop

def get_code_end(data):
	pos = -1
	while 1:
		res = data.find(b'\x1B\x00\x00\x00', pos+1)
		if res == -1:
			break
		pos = res
	return pos + 4
	
def parse_hdr(hdr):
	hdrtext = hdr[:0x1C].rstrip(b'\x00').decode('cp932')
	defines = {}
	entries, = struct.unpack('<I', hdr[0x24:0x28])
	pos = 0x28
	for k in range(entries):
		pos1 = hdr.find(b'\x00', pos)
		name = hdr[pos:pos1].decode('cp932')
		pos = pos1 + 1
		offset, = struct.unpack('<I', hdr[pos:pos+4])
		defines[offset] = name
		pos += 4
	return hdrtext, defines

def parse(code, hdr):
	if hdr:
		hdrtext, defines = parse_hdr(hdr)
	else:
		hdrtext = None
		defines = {}
	bgiop.clear_offsets()
	inst = {}
	size = get_code_end(code)
	pos = 0
	while pos < size:
		addr = pos
		op, = struct.unpack('<I', code[addr:addr+4])
		if op not in bgiop.ops:
			raise Exception('size unknown for op %02x @ offset %05x' % (op, addr))
		pos += 4
		fmt, pfmt, fcn = bgiop.ops[op]
		if fmt:
			n = struct.calcsize(fmt)
			args = struct.unpack(fmt, code[pos:pos+n])
			if fcn:
				args = fcn(code, addr, defines, *args)
			inst[addr] = pfmt % args
			pos += n
		else:
			inst[addr] = pfmt
	offsets = bgiop.offsets.copy()
	return inst, offsets, hdrtext, defines
	
def out(fo, inst, offsets, hdrtext, defines):
	if hdrtext:
		fo.write('#header "%s"\n\n' % asdis.escape(hdrtext))
	if defines:
		for offset in sorted(defines):
			fo.write('#define %s L%05x\n' % (defines[offset], offset))
		fo.write('\n')
	for addr in sorted(inst):
		if inst[addr].startswith('line('):
			fo.write('\n')
		if addr in offsets or addr in defines:
			if addr in defines:
				fo.write('\n%s:\n' % defines[addr])
			else:
				fo.write('\nL%05x:\n' % addr)
		fo.write('\t%s;\n' % inst[addr])
		
def dis(file):
	ofile = os.path.splitext(file)[0] + '.bsd'
	fi = open(file, 'rb')
	hdr_test = fi.read(0x20)
	if hdr_test.startswith(b'BurikoCompiledScriptVer1.00\x00'):
		hdrsize = 0x1C + struct.unpack('<I', hdr_test[0x1C:0x20])[0]
	else:
		hdrsize = 0
	fi.seek(0, 0)
	hdr = fi.read(hdrsize)
	code = fi.read()
	fi.close()
	inst, offsets, hdrtext, defines = parse(code, hdr)
	fo = open(ofile, 'w', encoding='utf-8')
	out(fo, inst, offsets, hdrtext, defines)
	fo.close()

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print('Usage: bgidis.py <file(s)>')
		print('(only extension-less files amongst <file(s)> will be processed)')
		sys.exit(1)
	for arg in sys.argv[1:]:
		for script in glob.glob(arg):
			base, ext = os.path.splitext(script)
			if not ext and os.path.isfile(script):
				print('Disassembling %s...' % script)
				dis(script)
