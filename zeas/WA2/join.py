import pdb
import os,sys

src = open(sys.argv[1],'rb').read().decode("utf16").replace('\r\n','\n').split('\n')
ignoreChar = {
    u'　': u'　',
    u'「': u'「',
    u'『': u'『',
    u'（': u'（',
    u'！': u'！',
    u'？': u'？',
    u'…': u'…',
    u'—': u'—',
    u'─': u'—',
    u'，': u'，',
    u'。': u'。',
    u'～': u'～',
    u'」': u'」',
    u'』': u'』',
    u'）': u'）',
    u'\\': u'\\',
    u'k' : u'k',
}
text=[]
pLine = 0
while pLine<len(text):
	line = text[pLine]
	pLine += 1
	if line and line[0] in u'●○':
		sig = line[0]
		sectance = []
		line = text[pLine]
		while not(line and line[0] in u'●○◆'):
			sentance.append(line)
			pLine += 1
			line = text[pLine]
		if sig == u'●':#句尾补全
			origsen = text[-1]
			if(len(sentance)==len(origsen)):
				for n in range(len(origsen)):
					o = origsen[n]
					l = sentance[n]
					p = len(o) - 1
					t = 0
					while(p and origsen[p-t] in ignoreChar):
						#################
					sentance[]
				text[-1] = sentance
		else:
			text.append(sentance)
			