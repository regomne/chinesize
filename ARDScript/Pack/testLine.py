import os

path1='New'
path2='New2'

files1=os.listdir(path1)
for f in os.listdir(path2):
    if f not in files1: continue
    f1=open(path1+os.sep+f,'rb')
    f2=open(path2+os.sep+f,'rb')
    lines1=f1.read().decode('utf-16').split('\r\n')
    lines2=f2.read().decode('utf-16').split('\r\n')
    if len(lines1)!=len(lines2):
        print f
    f1.close()
    f2.close()
