#encoding=gbk
import AinReader
import strfile
import zlib
import struct
import pdb

def ConvertFuncName(funcs):
  newf=[]
  for func in funcs:
    name=func[1].decode('932').encode('936','replace')
    newf.append((func[0],name,func[2],func[3],func[4],func[5],func[6],func[7],func[8]))
  return newf

def PackStruct(cntt,pack):
  for elem in cntt:
    if (type(elem) is tuple) or (type(elem) is list):
      PackStruct(elem,pack)
    elif type(elem) is str:
      pack.append('%s\0'%elem)
    elif (type(elem) is int) or (type(elem) is long):
      pack.append(struct.pack('I',elem))
    else:
      pdb.set_trace()
  return ''.join(pack)

fs=file('r82.ain','rb')
stm=strfile.MyStr(fs.read())
fs.close()

Segments={}
while(stm.tell()<len(stm)-1):
  flag=stm.read(4)
  if flag in AinReader.AINFUNC:
    Segments[flag]=AinReader.AINFUNC[flag](stm)
    print '%s segment processing successed.'%flag
  else:
    print 'Unknown segment flag "%s".'%flag
    break


fs=file('r82MSG0.txt','rb')
lines=fs.read().replace('\r\n','\n').split('\n')
if lines[-1]!='': lines.append('')
msg0='\0'.join(lines)
fs.close()

fs=file('r82STR0.txt','rb')
lines=fs.read().replace('\r\n','\n').split('\n')
if lines[-1]!='': lines.append('')
for i in range(len(lines)):
  lines[i]=lines[i].replace('\\n','\n')
str0='\0'.join(lines)
fs.close()

newfile=[]
for flag,seg in Segments.items():
  newfile.append(flag)
  if flag=='MSG0':
    newfile.append(stm[seg[0]:seg[0]+4])
    newfile.append(msg0)
  elif flag=='STR0':
    newfile.append(stm[seg[0]:seg[0]+4])
    newfile.append(str0)
  elif flag=='FUNC':
    newfile.append(stm[seg[0]:seg[0]+4])
    newfuncs=[]
    newseg=PackStruct(ConvertFuncName(seg[2]),newfuncs)
    newfile.append(newseg)
  else:
    newfile.append(stm[seg[0]:seg[0]+seg[1]])
newfile=''.join(newfile)


fs=file('r82new.ain','wb')
fs.write(newfile)
fs.close()

comp=zlib.compress(newfile)
newstm='AI2\x00\x00\x00\x00\x00'+struct.pack('II',len(newfile),len(comp))+comp
fs=file('RanceQuest.ain','wb')
fs.write(newstm)
fs.close()
