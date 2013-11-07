import os,sys
import pdb

if len(sys.argv)<2:
	print 'input.txt output.txt'
	sys.exit()

def splitnum(s):
	for n in xrange(len(s)):
		if s[n] not in '0123456789':
			break
	if s[n] in '0123456789': n += 1
	return int(s[:n]), s[n:]

new = []
count = 1
origcount = 0
temp = []
for line in open(sys.argv[1],'rb').read().decode('utf16').split('\r\n'):
	if line and line[0] in u'○●':
		sym = line[0]
		p = line.index(sym, 1)
		t, ex = splitnum(line[1:p])
		if t != origcount:
			if temp:
				for n in temp:
					new.append(n[0]+'%04d'%count+n)
					if n[0] == u'●':
						new.append(u';'+u'━'*32)
				count += 1
				temp = []
			origcount = t
		line = line[p:]
		if line[1:] == '\\n': continue
		elif ex == ':NAME': line = line[0] +'{'+ line[1:]+'}'
		elif line[1:3] == '\\n': line = line[0]+line[3:]
		temp.append(line)
for n in temp:
	new.append(n[0]+'%04d'%count+n)
open(sys.argv[2], 'wb').write('\r\n'.join(new).encode("utf16"))