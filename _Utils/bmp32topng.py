from PIL import Image
import struct
import os
from pdb import set_trace as int3

def extract_alpha_chan(dib, width, height, is_bottom):
  ch=[]
  stride = width * 4
  if not is_bottom:
    line = stride * (height - 1)
    for i in range(height):
      p = line
      for j in range(width):
        ch.append(255 - dib[p + 3])
        p += 4
      line -= stride
  else:
    line = 0
    i = 0
    for i in range(height):
      p = line
      for j in range(width):
        ch.append(255 - dib[p + 3])
        p += 4
      line += stride
  return bytes(ch)

def read_bmp(fname):
  fs=open(fname,'rb')
  magic, file_size, _, _, off_bits = struct.unpack('<HIHHI',fs.read(14))
  self_size, width, height, planes, bit_count, comp, image_size, _,_,_,_ =\
             struct.unpack('<IiiHHIIIIII',fs.read(40))
  if bit_count != 32:
    fs.seek(0)
    im = Image.open(fs)
    im.load()
    fs.close()
    return im
  else:
    fs.seek(0)
    im = Image.open(fs)
    im.load()

    fs.seek(off_bits)
    stride = width * 4;
    is_bottom = False
    if height < 0:
      height = -height;
      is_bottom = True
    dibs = fs.read(stride * height)
    fs.close()
    #int3()
    r,g,b = im.split()
    alpha = extract_alpha_chan(dibs, width, height, is_bottom)
    a = Image.frombytes('L', (width, height), alpha)
    return Image.merge('RGBA',(r,g,b,a))

def cvt_all(folder1, folder2):
  for f in os.listdir(folder1):
    if not f.endswith('.BMP'):
      continue
    pic = read_bmp(os.path.join(folder1,f))
    pic.save(os.path.join(folder2,f.replace('.BMP','.png')))
    pic.close()

cvt_all(r'd:\galgame\fortissimo FA\advdata\GRP\SYSTEM_pic',
        r'd:\galgame\fortissimo FA\advdata\GRP\SYSTEM_png')
