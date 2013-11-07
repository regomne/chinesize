#encoding=utf-8
#py3.2

import os

path1='1nss'
path2='nss2'

dirs=os.listdir(path2)

for f in os.listdir(path1):
    fs=open(path1+os.sep+f,'rb')
    len1=fs.read().count(b'\r\n')
    fs.close()
    fname=f.replace('.txt','完成.txt')
    if fname not in dirs: continue
    fs=open(path2+os.sep+fname,'rb')
    len2=fs.read().count(b'\r\x00\n\x00')
    if len1!=len2:
        print(f)
