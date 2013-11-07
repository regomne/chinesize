#py2.7
import os

path0='ardscript'
path2='new2'
path3='new3'
if path3 not in os.listdir('.'):
    os.mkdir(path3)
files2=os.listdir(path2)
for name in os.listdir(path0):
    if name not in files2: continue
    f0=open(path0+os.sep+name,'rb')
    f2=open(path2+os.sep+name,'rb')
    lines0=f0.read().decode('utf-16').replace(u'\r\n',u'\n').split('\n')
    lines2=f2.read().decode('utf-16').replace(u'\r\n',u'\n').split('\n')
    f0.close()
    f2.close()
    
    newtxt=[]
    i2=0
    for line in lines0:
        if len(line)<3:
            newtxt.append(line)
        elif line[0:2]==u'\\w':
##            newtxt.append(line[2:line.find(u'\\h')])
            nl=line[0:2]+lines2[i2]+line[line.find(u'\\h'):]
            i2+=1
            newtxt.append(nl)
        elif ord(line[0].encode('gbk')[0])>0x80:
##            newtxt.append(line[0:line.find(u'\\h')])
            nl=lines2[i2]+line[line.find(u'\\h'):]
            i2+=1
            newtxt.append(nl)
        elif line[0]=='"':
##            newtxt.append(line[1:line.find(u'"',1)])
            nl='"'+lines2[i2]+line[line.find(u'"',1):]
            i2+=1
            newtxt.append(nl)
        else:
            newtxt.append(line)
    fs=file(path3+os.sep+name,'wb')
    fs.write(u'\r\n'.join(newtxt).encode('utf_16'))
    fs.close()
