import os
import shutil

path='txt'

fnameList=os.listdir(path)

list2=[]
for f in fnameList:
    if f.endswith('.txt'):
        list2.append(int(f[0:-4],16))

list2.sort()
i=0
for f in list2:
    shutil.move(os.path.join(path,'%08x.txt'%f),os.path.join(path,'%02d.txt'%i))
    shutil.move(os.path.join(path,'%08x.tbl'%f),os.path.join(path,'%02d.tbl'%i))
    i+=1
