# BGI script file opcode table

import asdis
import re

re_fcn = re.compile('([A-Za-z_][A-Za-z0-9_:]*)\(.*\)')

offsets = set()

def clear_offsets():
	offsets.clear()

def get_string(code, addr, defines, *args):
	pos0 = args[0]
	pos1 = code.find(b'\x00', pos0)
	string = code[pos0:pos1].decode('cp932')
	string = asdis.escape(string)
	return string
	
def get_file(code, addr, defines, *args):
	pos0 = args[0]
	pos1 = code.find(b'\x00', pos0)
	string = code[pos0:pos1].decode('cp932')
	string = asdis.escape(string)
	lno = args[1]
	return string, lno
	
def get_offset(code, addr, defines, *args):
	offset = args[0]
	offsets.add(offset)
	if offset in defines:
		offset_s = defines[offset]
	else:
		offset_s = 'L%05x' % offset
	return offset_s
	
ops = {
0x000: ('<i', 'push_dword(%d)', None),
0x001: ('<I', 'push_offset(%s)', get_offset),
0x002: ('<i', 'push_base_offset(%d)', None),
0x003: ('<I', 'push_string("%s")', get_string),
0x008: ('<i', 'load(%d)', None),
0x009: ('<i', 'move(%d)', None),
0x00A: ('<i', 'move_arg(%d)', None),
0x010: ('', 'load_base()', None),
0x011: ('', 'store_base()', None),
0x018: ('', 'jmp()', None),
0x019: ('<I', 'jc(%#x)', None),
0x01A: ('', 'call()', None),
0x01B: ('', 'ret()', None),
0x01E: ('', 'reg_exception_handler()', None),
0x01F: ('', 'unreg_exception_handler()', None),
0x020: ('', 'add()', None),
0x021: ('', 'sub()', None),
0x022: ('', 'mul()', None),
0x023: ('', 'div()', None),
0x024: ('', 'mod()', None),
0x025: ('', 'and()', None),
0x026: ('', 'or()', None),
0x027: ('', 'xor()', None),
0x028: ('', 'not()', None),
0x029: ('', 'shl()', None),
0x02A: ('', 'shr()', None),
0x02B: ('', 'sar()', None),
0x030: ('', 'eq()', None),
0x031: ('', 'neq()', None),
0x032: ('', 'leq()', None),
0x033: ('', 'geq()', None),
0x034: ('', 'lt()', None),
0x035: ('', 'gt()', None),
0x038: ('', 'bool_and()', None),
0x039: ('', 'bool_or()', None),
0x03A: ('', 'bool_zero()', None),
0x03F: ('<i', 'nargs(%d)', None),
0x07F: ('<Ii', 'line("%s", %d)', get_file),
}

rops = {}

def make_ops():
	for op in range(0x400):
		if op not in ops:
			if op < 0x100:
				ops[op] = ('', 'f_%03x()' % op, None)
			elif 0x100 <= op < 0x140:
				ops[op] = ('', 'sys_::f_%03x()' % op, None)
			elif 0x140 <= op < 0x160:
				ops[op] = ('', 'msg_::f_%03x()' % op, None)
			elif 0x160 <= op < 0x180:
				ops[op] = ('', 'slct::f_%03x()' % op, None)
			elif 0x180 <= op < 0x200:
				ops[op] = ('', 'snd_::f_%03x()' % op, None)
			elif 0x200 <= op < 0x400:
				ops[op] = ('', 'grp_::f_%03x()' % op, None)
				
def make_rops():
	for op in ops:
		fcn, = re_fcn.match(ops[op][1]).groups()
		rops[fcn] = op
	
make_ops()
make_rops()
