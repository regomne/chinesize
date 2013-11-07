import os
import shutil

path1='fini'
path2='final'

for f in os.listdir(path1):
    if f.endswith('.txt.out.bin'):
        shutil.move(os.path.join(path1,f),os.path.join(path2,f.replace('.txt.out.bin','')))
