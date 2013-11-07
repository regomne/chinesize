import os

path2=r'D:\chinesize\yuris\ys2t'
path1=r'D:\chinesize\yuris\ys2tt'

dir2=os.listdir(path2)
for f in os.listdir(path1):
    if f not in dir2:
        print("Not in: "+f)
        continue
    fs=open(os.path.join(path1,f),'rb')
    stm=fs.read()
    if stm[0]!=0xff:
        print("Code Error: "+f);
        continue
    lines1=stm.split(b'\r\x00\n\x00')
    fs=open(os.path.join(path2,f),'rb')
    stm=fs.read()
##    if stm[0]!=0xff:
##        print("Code Error: "+f);
##        continue
    lines2=stm.split(b'\r\x00\n\x00')
    if len(lines1)!=len(lines2):
        print("Not Match: "+f)
##    if lines1[0]!=lines2[0]:
##        print("translated: "+f)
