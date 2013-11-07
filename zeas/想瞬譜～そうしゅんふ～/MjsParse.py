class Reader:
	buff = ''
	pt = 0
	def __init__(self, buff):
		self.pt = 0
		self.buff = buff
	
	def read(self, bytes = 1):
		ch = self.peek(bytes)
		if self.pt < len(self.buff): self.pt += bytes
		return ch
	
	def peek(self, bytes = 1):
		ch = self.buff[self.pt:self.pt + bytes]
		return ch
	
	def tell(self): return self.pt
	
	def back(self):#go back 1 char
		if self.pt: self.pt -= 1
		return self.pt

class Token:
	line = 0
	value = 0
	type = 0
	
	def __init__(self, line = 0, type = 'EOL', value = ''):
		self.line = line
		self.type = type
		self.value = value
	
	def __repr__(self):
		return '<%s %s [%d]>'%(self.type, repr(self.value), self.line)
	
	def toInst(self):
		return struct.pack('I', self.line) + chr(self.type) + self.value.encode('shiftjis') + '\0'

class Lexer:
	rd = ''
	line = 0
	col = 0
	_token = 0

	_CHR_DECNUM = '0123456789'
	_CHR_HEXNUM = _CHR_DECNUM + 'abcdefABCDEF'
	_CHR_IDFIRST = '_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
	_CHR_ID = _CHR_IDFIRST + _CHR_DECNUM
	
	def __init__(self, buff):
		self.rd = Reader(buff)
		self.line = 1
		self.col = 1
	
	def next(self):
		self._token = self._next()
		return self._token
	
	def token(self):
		if not self._token:
			self._token = self._next()
		return self._token
	
	def readString(self, quote):
		text = []
		while(1):
			ch = self.rd.read()
			self.col += 1
			if ch == quote : break
			assert(ch and ch not in '\r\n')
			if ch == '\\':
				ch = self.rd.read()
				if ch == 'a':
					text.append('\a')
				elif ch == 'b':
					text.append('\b')
				elif ch == 'f':
					text.append('\f')
				elif ch == '\\':
					text.append('\\')
				elif ch == '\'':
					text.append('\'')
				elif ch == '"':
					text.append('"')
				elif ch == 'r':
					text.append('\r')
				elif ch == 'n':
					text.append('\n')
				elif ch == 't':
					text.append('\t')
				elif ch == 'v':
					text.append('\v')
				elif ch == 'x':
					ch = self.rd.read()
					assert(ch in self._CHR_HEXNUM)
					num = int(ch, 16)
					if self.rd.peek() in self._CHR_HEXNUM:
						num = num << 16 + int(self.rd.read(), 16)
					text.append(chr(num))
				else:
					raise(Exception('Unexpected escape chara "\\x%s"'%ch))
			else:
				text.append(ch)
		return ''.join(text)
	
	def skipComment(self):
		assert(self.rd.read(2) == '{-')
		self.col += 2
		while(self.rd.peek(2) != '-}'):
			ch = self.rd.read()
			assert(ch)
			self.col+=1
			if ch in '\r\n':
				line += 1
				self.col = 1
		assert(self.rd.read())
		self.col+=1
	
	def readIdent(self, ch):
		ident = [ch]
		while self.rd.peek() in self._CHR_ID:
			ident.append(self.rd.read())
			self.col += 1
		return ''.join(ident)
	
	def readHex(self):
		h = []
		while self.rd.peek() in self._CHR_HEXNUM:
			h.append(self.rd.read())
			self.col += 1
		return ''.join(h)
	
	def readNum(self, ch):
		n = [ch]
		while self.rd.peek() in self._CHR_DECNUM:
			n.append(self.rd.read())
			self.col += 1
		return Token(self.line, 'INT', int(''.join(n)))
	
	def _next(self):
		while(1):
			ch = self.rd.read()
			if not ch: return Token(self.line, 'EOL', 'EOF')
			self.col += 1
			
			if ch in ' \t': continue
			elif ch in '\r\n':
				self.line += 1
				self.col = 1
				return Token(self.line, 'EOL', '')
			elif ch == '{':
				if self.rd.peek() == '-':
					self.rd.back()
					self.col -= 1
					self.skipComment()
					continue
				else:
					return Token(self.line, '{', 'token')
			elif ch == '#' and self.rd.peek() in self._CHR_IDFIRST:
				self.col += 1
				ident = ch + self.readIdent(self.rd.read()).lower()
				type = {
					"#entrypoint": 'DENTRYPOINT',
					"#function":   'DFUNCTION',
					"#res":        'DRES'
				}[ident]
				return Token(self.line, type, ident)
			elif ch == '[' and self.rd.peek() == '#':
				self.rd.read()
				self.col += 1
				tk = Token(self.line, 'BYTES', self.readHex().decode('hex'))
				assert(self.rd.read()==']')
				self.col += 1
				return tk
			elif ch == '[' and self.rd.peek() == ']': #empty bytes
				self.rd.read()
				self.col += 1
				return Token(self.line, 'BYTES', '')
			elif ch == '$':
				return Token(self.line, 'INT', int(self.readHex(),16))
			elif ch in '\'"':
				return Token(self.line, 'STR', self.readString(ch))
			elif ch == '@':
				self.col += 1
				return Token(self.line, 'LABEL', self.readIdent(self.rd.read()))
			elif ch in ',[](){}<>': return Token(self.line, ch, ch)
			elif ch in self._CHR_DECNUM:
				return self.readNum(ch)
			elif ch in '+-' and self.rd.peek() in self._CHR_DECNUM:
				return self.readNum(ch)
			elif ch == 'o' and self.rd.peek() == 'p':
				return Token(self.line, 'OP', int(self.readIdent(ch)[2:], 16))
			elif ch == 'j' and self.rd.peek(2) == 'mp':
				return Token(self.line, 'JMP', int(self.readIdent(ch)[3:], 16))
			else:
				token = self.readIdent(ch)
				if token in keywords:
					token = keywords[token]
					return Token(self.line, token[0], token[1])
				return Token(self.line, token, 'token')

keywords = {
	'jne':             ('JMP',    0x831),
	'goto':            ('JMP',    0x82c),
    "text_control_66": ('TEXTCMD', 0x66),
    "br":              ('TEXTCMD', 0x6e),
    "text_control_6f": ('TEXTCMD', 0x6f),
    "pause":           ('TEXTCMD', 0x70),
    "text_control_72": ('TEXTCMD', 0x72),
    "text_control_73": ('TEXTCMD', 0x73),
    "cls":             ('TEXTCMD', 0x77),
}

class Statement:
	def __init__(self, type, ctx):
		self.type = type
		self.ctx = ctx
		
	def __repr__(self):
		return '<%s %s>'%(self.type, repr(self.ctx))

class Parser:
	def stat_labels(self, n):
		i = 0
		ctx = []
		while self.lexbuf[n+i].type == 'LABEL':
			ctx.append(Statement(self.lexbuf[n+i].type, self.lexbuf[n+i].value))
			i += 1
			if len(self.lexbuf) <= n+i+1:
				self._preload(2)
			if self.lebuf[n+i].type != ',':
				break
#			ctx.append(Statement(self.lexbuf[n+i].type, self.lexbuf[n+i].value))
			i += 1
		return i, i, ctx

	def stat_expr(self, i):
		for stat in self.Statements:
			type = stat[0]
			if type not in ('Op', 'Call', 'SysCall', 'Label', 'Str', 'Int', 'Pop'):
				continue
			stat = stat[1:]
			n, ctx = self._match(stat, i)
			if not n: continue
			return n, n, [Statement(type, ctx)]
		return False, 0, []
	
	def stat_exprs(self, n):
		ctx = []
		ok, n, c = self.stat_expr(n)
		while ok:
			ctx += c
			self._preload(2)
			if self.lexbuf[n].type != ',':
				break
			ok, n, c = self.stat_expr(n+1)
		return ok, n, ctx
	
	def stat_params(self, n):
		if self.lexbuf[n].type != '(':
			return True, 0, [Statement('Param', [])]
		ok, n, params =  self.stat_exprs(n+1)
		if self.lexbuf[n].type != ')':
			pdb.set_trace()
		return True, n + 1, [Statement('Param', params)]
	
	def __init__(self, buff):
		self.lex = Lexer(buff)
		self.lexbuf = []
		self._statement = []
		self.Statements = [
			('EOL', 'EOL',),
			('Entrypoint', 'DENTRYPOINT', 'INT'),
			('Function', 'DFUNCTION', 'INT'),
			('Exit', 'exit', Parser.stat_params),
			('Call', 'call', '<', 'INT', ',', 'INT', '>', Parser.stat_params),
			('SysCall', 'syscall', '<', 'INT', '>', Parser.stat_params),
			('SysCall', 'syscall', '<', 'INT', '>'),
			('Res', 'DRES', '<', 'INT', '>'),
			('Int', 'INT',),
			('Str', 'STR',),
			('Label', 'LABEL',),
			('Switch', 'switch', '{', Parser.stat_labels, '}'),
			('Push', 'push', Parser.stat_expr),
			('Jmps', 'JMP', Parser.stat_params, 'LABEL'),
			('Jump', 'JMP', 'LABEL'),
			('Op', 'OP', 'BYTES'),
			('Op', 'OP',),
			('Pop', 'pop',),
		]
	
	def _preload(self, n):
		for n in range(n):
			tk = self.lex.next()
			if tk.type == 'EOL' and tk.value == 'EOF':
				return False
			self.lexbuf.append(tk)
		return True
	
	def _match(self, stat, n = 0):
		if len(self.lexbuf)<len(stat) + n:
			self._preload(n + len(stat) - len(self.lexbuf))
			if not self.lexbuf:
				return -1, 0
			if self.lexbuf[0].type == 'EOL':
				self.lexbuf = self.lexbuf[1:]
		ctx = []
		for cmp in stat:
			if type(cmp) in (str, unicode):
				if self.lexbuf[n].type != cmp:
					return False, ctx
				ctx.append(self.lexbuf[n].value)
				n += 1
			else:
				ok, n, c = cmp(self, n)
				if not ok: return False, ctx
				ctx += c
		return n, ctx
	
	def _next(self):
		while self.lexbuf:
			tk = self.lexbuf[0]
			if tk.type != 'EOL':
				break
			self.lexbuf = self.lexbuf[1:]
		for type in self.Statements:
			stat = type[1:]
			type = type[0]
			n, ctx = self._match(stat + ('EOL',))
			if not n: continue
			elif n == -1: return Statement('EOL', 'EOF')
			self.lexbuf = self.lexbuf[n:]
			return Statement(type, ctx[:-1])
		pdb.set_trace()
		raise Exception('no statement matched.')
	
	def next(self):
		self._statement = self._next()
		return self._statement
		
	def stat(self):
		if not self._statement:
			self._statement = self._next()
		return self._statement

import pdb