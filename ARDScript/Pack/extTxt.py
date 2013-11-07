#py2.7
import os

chrs=0
if 'New' not in os.listdir('.'):
    os.mkdir('New')
for name in os.listdir('ArdScript'):
    fs=file('ArdScript\\'+name,'rb')
    lines=fs.read().decode('utf-16').replace(u'\r\n',u'\n').split('\n')
    fs.close()
    newtxt=[]
    for line in lines:
        if len(line)<3: continue
        if line[0:2]==u'\\w':
            newtxt.append(line[2:line.find(u'\\h')])
        elif ord(line[0].encode('gbk')[0])>0x80:
            newtxt.append(line[0:line.find(u'\\h')])
        elif line[0]=='"':
            newtxt.append(line[1:line.find(u'"',1)])
    fs=file('New\\'+name,'wb')
    fs.write(u'\r\n'.join(newtxt).encode('utf_16'))
    fs.close()
