#py2.7
import strfile
import struct

fs=file('haru_c.ain','rb')
stm=strfile.MyStr(fs.read())
stm.seek(stm.find('STR0')+4)

count,=struct.unpack('I',stm.read(4))
newtxt=[]
cur=stm.tell()
for i in range(count):
    len1=stm[cur:].find('\0')
    newtxt.append(stm[cur:cur+len1].replace('\n','\\n'))
    cur+=len1+1

fs=file('r82STR0.txt','wb')
fs.write('\r\n'.join(newtxt))
fs.close()
