import os
import sys

g_cvt_tool='convert'

def merge(dzi_path, dzi_name, dzi_prefix):
  fs=open(os.path.join(dzi_path,dzi_name), 'rb')
  ls=fs.read().decode().split('\r\n')
  fs.close()
  if ls[0]!='DZI':
    raise Exception('not a dzi file')
  width,height=[int(v) for v in ls[1].split(',')]
  img_cnt=int(ls[2])
  i=3
  for img_idx in range(img_cnt):
    cmd=[g_cvt_tool, '-background', 'transparent']
    
    blockx,blocky=[int(v) for v in ls[i].split(',')]
    i+=1
    py=0
    for j in range(blocky):
      px=0
      fnames=ls[i].split(',')
      i+=1
      if len(fnames)!=blockx:
        raise Exception('block error')
      for f in fnames:
        if len(f)!=0:
          cmd+=['-page','+%d+%d'%(px,py), os.path.join(dzi_path,f,'.png')]
        px+=256
      py+=256
    cmd+=['-mosaic', '%s_%02d.png'%(dzi_prefix, img_idx)]
    os.system(' '.join(cmd))


