#py3.2

import zlib
import os

path1='a'
path2='exp'
path3='ori'

for f in os.listdir(path1):
    fs=open(path1+os.sep+f,'rb')
    stm=fs.read()
    if stm[0:8]!=b'CatScene':
##        fs=open(path3+os.sep+f,'wb')
##        fs.write(stm)
        continue
    news=zlib.decompress(stm[16:])
    fs=open(path2+os.sep+f,'wb')
    fs.write(news)
    fs.close()
