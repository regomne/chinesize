import os
import sys
import shutil
import subprocess

g_cvt_tool=r'D:\Program Files\ImageMagick-6.2.7-Q8\convert.exe'

def merge(dzi_path, dzi_name, outdir):
  dzi_prefix=dzi_name.split('.')[0]
  os.chdir(dzi_path)
  fs=open(dzi_name, 'rb')
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
    if blockx==1 and blocky==1:
      f=ls[i]
      i+=1
      shutil.copy(os.path.join('tex',f+'.png'),os.path.join(outdir,'%s_%02d.png'%(dzi_prefix, img_idx)))
      print(dzi_prefix,'copied')
    else:
      py=0
      for j in range(blocky):
        px=0
        fnames=ls[i].split(',')
        i+=1
        if len(fnames)!=blockx:
          raise Exception('block error')
        for f in fnames:
          if len(f)!=0:
            cmd+=['-page','+%d+%d'%(px,py), os.path.join('tex',f+'.png')]
          px+=256
        py+=256
      cmd+=['-mosaic', os.path.join(outdir,'%s_%02d.png'%(dzi_prefix, img_idx))]
      #print(cmd)
      out=subprocess.run(cmd, stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
      if out.returncode!=0:
        print(dzi_prefix,'error!')
        print(out.stdout.decode())
      else:
        print(dzi_prefix,'converted')

#merge(r'f:\chinesize\Malie\split_dzi','award_base.dzi','award_base')
rootdir=r'g:\Program Files\sousyu1\data9\screen'
outdir=r'g:\Program Files\sousyu1\data9\out'
fs=open(r'g:\Program Files\sousyu1\data9\screen\list.txt','rb')
ls=fs.read().decode().split('\r\n')
fs.close()
for dzi in ls:
  base=os.path.dirname(dzi)
  newdir=os.path.join(outdir,base)
  if not os.path.exists(newdir):
    os.makedirs(newdir)
  merge(os.path.join(rootdir,os.path.dirname(dzi)),os.path.basename(dzi), newdir)

