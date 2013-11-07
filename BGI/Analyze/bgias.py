#!/usr/bin/env python3

# BGI script file assembler

import glob
import os
import struct
import sys
import base64

import asdis
import bgiop

def parse_instr(line, n):
	strings = set([])
	fcn, argstr = asdis.re_instr.match(line).groups()
	argstr = argstr.strip()
	if argstr:
		argstr = argstr.replace('\\\\', asdis.backslash_replace).replace('\\"', asdis.quote_replace)
		quotes = asdis.get_quotes(argstr, n)
		if len(quotes) % 2 != 0:
			raise asdis.QuoteMismatch('Mismatched quotes @ line %d' % n)
		argstr = asdis.replace_quote_commas(argstr, quotes)
		args = [x.strip().replace(asdis.comma_replace, ',').replace(asdis.quote_replace, '\\"').replace(asdis.backslash_replace, '\\\\') for x in argstr.split(',')]
		for arg in args:
			if arg and arg[0] == '"' and arg[-1] == '"':
				strings.add(arg)
	else:
		args = []
	return fcn, args, strings

def parse(asmtxt):
	instrs = []
	symbols = {}
	text_set = set()
	pos = 0
	hdrtext = None
	defines = {}
	for id, line in enumerate(asmtxt.split('\n')):
		line = line.strip()
		line = asdis.remove_comment(line)
		if not line:
			continue
		if asdis.re_header.match(line):
			hdrtext, = asdis.re_header.match(line).groups()
			hdrtext = asdis.unescape(hdrtext)
		elif asdis.re_define.match(line):
			name, offset_s = asdis.re_define.match(line).groups()
			defines[name] = offset_s
		elif asdis.re_label.match(line):
			symbol, = asdis.re_label.match(line).groups()
			symbols[symbol] = pos
		elif asdis.re_instr.match(line):
			fcn, args, strings = parse_instr(line, id+1)	
			record = fcn, args, pos, id+1
			text_set.update(strings)
			instrs.append(record)
			try:
				op = bgiop.rops[fcn]
			except KeyError:
				raise asdis.InvalidFunction('Invalid function @ line %d' % (id+1))
			pos += struct.calcsize(bgiop.ops[op][0]) + 4
		else:
			raise asdis.InvalidInstructionFormat('Invalid instruction format @ line %d' % (id+1))
	texts = []
	for text in text_set:
		symbols[text] = pos
		text = asdis.unescape(text[1:-1])
		texts.append(text)
		pos += len(text.encode('cp936')) + 1
	return instrs, symbols, texts, hdrtext, defines
	
def out_hdr(fo, hdrtext, defines, symbols):
	fo.write(hdrtext.encode('cp936').ljust(0x1C, b'\x00'))
	entries = len(defines)
	hdrsize = 12 + 4*entries
	hdrsize += sum(len(name.encode('cp936'))+1 for name in defines)
	padding = ((hdrsize + 11) >> 4 << 4) + 4 - hdrsize
	hdrsize += padding
	fo.write(struct.pack('<III', hdrsize, 0, entries))
	for name in sorted(defines, key = lambda x: symbols[x]):
		fo.write(name.encode('cp936') + b'\x00')
		fo.write(struct.pack('<I', symbols[name]))
	fo.write(b'\x00'*padding)
	
def out(fo, instrs, symbols, texts, hdrtext, defines):
	if hdrtext:
		out_hdr(fo, hdrtext, defines, symbols)
	for fcn, args, pos, n in instrs:
		op = bgiop.rops[fcn]
		fo.write(struct.pack('<I', op))
		for arg in args:
			if arg in symbols:
				fo.write(struct.pack('<I', symbols[arg]))
			elif arg.startswith('0x') or arg.startswith('-0x'):
				fo.write(struct.pack('<i', int(arg, 16)))
			elif arg:
				fo.write(struct.pack('<i', int(arg)))
	for text in texts:
		fo.write(text.encode('cp936') + b'\x00')

def asm(file):
	ofile = os.path.splitext(file)[0]
	asmtxt = open(file, 'r', encoding='utf-8').read()
	instrs, symbols, texts, hdrtext, defines = parse(asmtxt)
	out(open(ofile, 'wb'), instrs, symbols, texts, hdrtext, defines)
	
if __name__ == '__main__':
	if len(sys.argv) < 2:
		print('Usage: bgias.py <file(s)>')
		print('(only .bsd files amongst <file(s)> will be processed)')
		sys.exit(1)
	for arg in sys.argv[1:]:
		for script in glob.glob(arg):
			base, ext = os.path.splitext(script)
			if ext == '.bsd':
				print('Assembling %s...' % script)
				asm(script)
