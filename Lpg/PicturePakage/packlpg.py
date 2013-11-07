#py3.2

import bytefile
import struct
import os

path1='newbmp'
path2='newlpg'

def ScanDib(dib,cnt):
    idx=[]
    for i in range(cnt):
        bgr=dib[i*4:i*4+3]
        if bgr in idx:
            continue
        else:
            idx.append(bgr)
    pallete={}
    for i in range(len(idx)):
        pallete[idx[i]]=i
    return (idx,pallete)

def PackLpg(bmpstm):
    magic, bfsize, offbits=struct.unpack('<2sI4xI',bmpstm.read(14))
    bisize, width, height, planes, bitcount, compr, sizeimage=\
        struct.unpack('IIIHHII16x',bmpstm.read(0x28))
    if bitcount!=32:
        print('bmp format error!')
        return
    dib=bytes(bmpstm[offbits:offbits+width*height*4])
    
    idx, pallete=ScanDib(dib,width*height)
    if len(pallete)>256:
        print('not a 256-color bmp!')
        return
    comprdib=bytearray(width*height*2)
    for i in range(width*height):
        bgr=dib[i*4:i*4+3]
        comprdib[i*2]=pallete[bgr]
        comprdib[i*2+1]=dib[i*4+3]

    pallete=bytearray()
    for ent in idx:
        pallete+=ent+b'\xcd'
    pallete.ljust(0x100*4,b'\x00')
    
    hdr=struct.pack('IIiIIII',1,bitcount,-1,0,width,height,len(comprdib))
    return hdr+pallete+comprdib


for f in os.listdir(path1):
    fs=open(os.path.join(path1,f),'rb')
    stm=bytefile.ByteIO(fs.read())
    print('Processing ',f)
    newstm=PackLpg(stm)
    fs=open(os.path.join(path2,f.replace('.bmp','.lpg')),'wb')
    fs.write(newstm)
    fs.close()
