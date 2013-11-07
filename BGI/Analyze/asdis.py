# Common assembler/disassembler routines

import re

comma_replace     = '@@@z@@Q@@@'
quote_replace     = '$$$q$$H$$$'
backslash_replace = '###g##V###'

re_label = re.compile(r'([A-Za-z_][A-Za-z0-9_]+):$')
re_instr = re.compile(r'([A-Za-z_][A-Za-z0-9_:]*)\((.*)\);$')
re_header = re.compile(r'#header\s+"(.+)"')
re_define = re.compile(r'#define\s+([A-Za-z0-9_]+)\s+([A-Za-z0-9_]+)')

class QuoteMismatch(Exception):
	pass
	
class InvalidInstructionFormat(Exception):
	pass
	
class InvalidFunction(Exception):
	pass

def escape(text):
	text = text.replace('\\', '\\\\')
	text = text.replace('\a', '\\a')
	text = text.replace('\b', '\\b')
	text = text.replace('\t', '\\t')
	text = text.replace('\n', '\\n')
	text = text.replace('\v', '\\v')
	text = text.replace('\f', '\\f')
	text = text.replace('\r', '\\r')
	text = text.replace('"', '\\"')
	return text
	
def unescape(text):
	text = text.replace('\\\\', backslash_replace)
	text = text.replace('\\a', '\a')
	text = text.replace('\\b', '\b')
	text = text.replace('\\t', '\t')
	text = text.replace('\\n', '\n')
	text = text.replace('\\v', '\v')
	text = text.replace('\\f', '\f')
	text = text.replace('\\r', '\r')
	text = text.replace('\\"', '"')
	text = text.replace(backslash_replace, '\\')
	return text

def remove_comment(line):
	cpos = 0
	while True:
		cpos = line.find('//', cpos)
		if cpos == -1:
			return line.rstrip()
		qcount = line[:cpos].count('"') - line[:cpos].count('\\"')
		if qcount % 2 == 0:
			break
		cpos += 1
	line = line[:cpos]
	return line.rstrip()
	
def get_quotes(line, n):
	pos = 0
	quotes = []
	while True:
		pos = line.find('"', pos)
		if pos == -1:
			break
		quotes.append(pos)
		pos += 1
	return quotes
	
def replace_quote_commas(line, quotes):
	pos = 0
	commas = []
	while True:
		pos = line.find(',', pos)
		if pos == -1:
			break
		for squote, equote in zip(quotes[::2], quotes[1::2]):
			if squote < pos < equote:
				commas.append(pos)
				break
		pos += 1
	commas.reverse()
	for pos in commas:
		line = line[:pos] + comma_replace + line[pos+1:]
	return line
