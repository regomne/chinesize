#encoding=utf-8
import strfile
import os
from struct import unpack
import pdb
import nsbParser

##ori="C:\\chinesize\\N2System\\SG\\nss\\"
##new='C:\\newchnsz\\N2System\\nsbparser\\parsed\\'

ori='F:\\Program Files\\STEINSGATE\\nssex\\'
new='F:\\Program Files\\STEINSGATE\\nssex\\'

files=os.listdir(ori)
for name in files:
  if name.endswith('.nsb'):
    cur=name
    fs=open(ori+name,'rb')
    stm=strfile.MyStr(fs.read())
    fs.close()
    fs=open(new+name.replace('.nsb','.txt'),'wb')
    par=nsbParser.NsbParser(stm)
    fs.write(par.Parse().decode('shift-jis','replace').encode('utf_16'))
    fs.close()
    print name

