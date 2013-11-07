import os,sys

textorg, textnew, textout = sys.argv[1:4]

def listunifile(filename):
	f = open(filename,'rb').read()
	if f[:2]=='\xff\xfe': f = f.decode('utf16')
	elif f[:2]=='\xfe\xff': f = f[2:].decode('utf_16_be')
	elif f[:3]=='\xef\xbb\xbf': f = f[3:].decode('utf8')
	else: f = f.decode('gb18030')
	return f.replace('\r\n','\n').split(u'\n')

orig = []
disp = []
for line in listunifile(textorg):
	if not line or line[0] in u';#': continue
	elif line[0] == u'○':
		orig.append((line[:line.index(line[0],1)+1], line[line.index(line[0],1)+1:]))
	elif line[0] == u'●':
		disp.append((line[:line.index(line[0],1)+1], line[line.index(line[0],1)+1:]))
tran = []
for line in listunifile(textnew):
	if not line or line[0] in u';#': continue
	elif line[0] == u'○':
		tran.append((line[:line.index(line[0],1)+1], line[line.index(line[0],1)+1:]))

if len(tran)!=len(orig):
	import pdb
	pdb.set_trace()

TEXT = []
for n in range(len(orig)):
	origline = orig[n][1]
	tranline = tran[n][1]
	if origline == tranline:
		displine = orig[n][0]+disp[n][1]
	else:
		displine = orig[n][0]+tranline
	line = ''.join(orig[n])
	TEXT.append(line)
	TEXT.append(displine)
	TEXT.append(';'+'='*63)
open(textout,'wb').write('\r\n'.join(TEXT).encode("utf16"))