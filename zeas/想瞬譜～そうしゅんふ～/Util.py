'''
   Utility functions and definitions.
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

import struct,sys

def read_int(ic):
	return struct.unpack('I', ic.read(4))[0]
def read_int32(ic):
	return struct.unpack('i', ic.read(4))[0]

def read_i16(ic):
	return struct.unpack('h', ic.read(2))[0]
read_real_i32 = read_int32

def getopts(para):
	p = 1
	args = {}
	files = []
	while p < len(sys.argv):
		arg = sys.argv[p]
		p += 1
		if arg.startswith('-'):
			arg = arg[1:]
			if arg.startswith('-'):
				arg = arg[1:]
			args[arg[0]] = ''
			if arg in para:
				arg = arg.split('=',1)
				if len(arg) == 1:
					if p < len(sys.argv):
						param = sys.argv[p]
						p += 1
						args[arg[0]] = param
				else:
					args[arg[0]] = arg[1]
		else:
			files.append(arg)
	return args, files