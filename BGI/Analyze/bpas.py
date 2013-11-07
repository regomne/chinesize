#!/usr/bin/env python3

# BGI ._bp file assembler

import glob
import os
import struct
import sys

import asdis
import bpop

def parse_instr(line, n):
	strings = set([])
	fcn, argstr = asdis.re_instr.match(line).groups()
	argstr = argstr.strip()
	if argstr:
		argstr = argstr.replace('\\\\', asdis.backslash_replace).replace('\\"', asdis.quote_replace)
		quotes = asdis.get_quotes(argstr, n)
		if len(quotes) %2 != 0:
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
	for id, line in enumerate(asmtxt.split('\n')):
		line = line.strip()
		line = asdis.remove_comment(line)
		if not line:
			continue
		if asdis.re_label.match(line):
			symbol, = asdis.re_label.match(line).groups()
			symbols[symbol] = pos
		elif asdis.re_instr.match(line):
			fcn, args, strings = parse_instr(line, id+1)
			record = fcn, args, pos, id+1
			text_set.update(strings)
			instrs.append(record)
			try:
				op = bpop.rops[fcn]
			except KeyError:
				raise asdis.InvalidFunction('Invalid function @ line %d' % (id+1))
			pos += struct.calcsize(bpop.ops[op][0]) + 1
		else:
			raise asdis.InvalidInstructionFormat('Invalid instruction format @ line %d' % (id+1))
	while pos % 0x10 != 0:
		pos += 1
	texts = []
	for text in text_set:
		symbols[text] = pos
		text = asdis.unescape(text[1:-1])
		texts.append(text)
		pos += len(text.encode('cp932')) + 1
	while pos % 0x10 != 0:
		pos += 1
	size = pos
	return instrs, symbols, texts, size

def out(fo, instrs, symbols, texts, size):
	hdr = struct.pack('<IIII', 0x10, size, 0, 0)
	fo.write(hdr)
	for fcn, args, pos, n in instrs:
		op = bpop.rops[fcn]
		fo.write(struct.pack('B', op))
		arglist = []
		for arg in args:
			if arg in symbols:
				arglist.append(symbols[arg]-pos)
			elif arg.startswith('0x') or arg.startswith('-0x'):
				arglist.append(int(arg, 16))
			elif arg:
				arglist.append(int(arg))
		fmt = bpop.ops[op][0]
		if fmt:
			fo.write(struct.pack(fmt, *tuple(arglist)))
	while fo.tell() % 0x10 != 0:
		fo.write(b'\x00')
	for text in texts:
		fo.write(text.encode('cp932') + b'\x00')
	while fo.tell() % 0x10 != 0:
		fo.write(b'\x00')

def asm(file):
	ofile = os.path.splitext(file)[0] + '._bp'
	asmtxt = open(file, 'r', encoding='utf-8').read()
	instrs, symbols, texts, size = parse(asmtxt)
	out(open(ofile, 'wb'), instrs, symbols, texts, size)

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print('Usage: bpas.py <file(s)>')
		print('(only .bpd files amongst <file(s)> will be processed)')
		sys.exit(1)
	for arg in sys.argv[1:]:
		for script in glob.glob(arg):
			base, ext = os.path.splitext(script)
			if ext == '.bpd':
				print('Assembling %s...' % script)
				asm(script)