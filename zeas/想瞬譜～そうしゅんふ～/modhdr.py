import os,sys

data = 'MjPlainBytecode\0'+open(sys.argv[1],'rb').read()[16:]
open(sys.argv[1],'wb').write(data)