import os

path1='txt'
path2='ntxt'

def cvtLine(line):
    nl=[]
    i=0
    if len(line)<=3:
        return line
    if line[-3:]=='\\01':
        line=line[0:-3]
    while i<len(line):
        c=line[i]
        nl.append(c)
        if (c=='…') or (c=='、'):
            if line[i+1:i+4]!='\\03':
                print('not 03')
                i+=1
                continue
            i+=4
            continue
        i+=1
    return ''.join(nl)

for f in os.listdir(path1):
    if not f.endswith('.txt'):
        continue
    print(f)
    ls=open(os.path.join(path1,f),'rb').read().decode('932').split('\r\n')
    nl=[]
    for l in ls:
        nl.append(cvtLine(l))
    open(os.path.join(path2,f),'wb').write('\r\n'.join(nl).encode('u16'))
