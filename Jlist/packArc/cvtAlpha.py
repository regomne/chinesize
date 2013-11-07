#encoding=utf-8
#py3.2

import os
from pdb import set_trace as int3
from struct import unpack,pack

def CombinePic(rgbf,af):
    magic, bmp_size, off_bits=unpack('<HI4xI',rgbf[0:14])
    hdr_size, width, height, planes, bit_count=unpack('<IIiHH',rgbf[14:30])

    if height>0 or bit_count!=24:
        return 0
    w=width
    h=-height
    Rgb=rgbf[off_bits:]
    
    magic, bmp_size, off_bits=unpack('<HI4xI',af[0:14])
    hdr_size, width, height, planes, bit_count=unpack('<IIiHH',af[14:30])
    if height>0 or bit_count!=8 or w!=width or h!=-height:
        return 0
    Aph=af[off_bits:]
    
    if len(Rgb)!=3*len(Aph) or len(Aph)!=w*h:
        return 0
    dib=bytearray(h*w*4)
    for i in range(h*w):
        dib[i*4:i*4+3]=Rgb[i*3:i*3+3]
        dib[i*4+3]=Aph[i]
    bfh=bytearray(pack('<2sIHHI',b'BM',len(dib)+0x36,0,0,0x36))
    bih=bytearray(pack('<IIiHH24s',0x28,w,-h,1,32,b'\x00'*24))
    return bfh+bih+dib

def SplitPic(rgba):
    magic,bmp_size,off_bits=unpack('<hI4xI',rgba[0:14])
    hdr_size, width, height, planes, bit_count=unpack('<IIiHH',rgba[14:30])

    if bit_count!=32:
        #int3()
        return 0
    w=width
    if height<0:
        h=-height
    else:
        h=height

    dib=rgba[off_bits:]
    if len(dib)<h*w*4:
        int3()
        return 0
    rgb=bytearray(h*w*3)
    aph=bytearray(h*w)
    for i in range(h*w):
        rgb[i*3:i*3+3]=dib[i*4:i*4+3]
        aph[i]=dib[i*4+3]

    pallete=bytearray()
    for i in range(0x100):
        pallete+=pack('BBBB',i,i,i,0)
    bfa=pack('<2sIHHI',b'BM',len(aph)+0x36+len(pallete),0,0,0x36+len(pallete))
    bia=pack('<IIiHH24s',0x28,w,height,1,8,b'\x00'*24)
    bmpa=bfa+bia+pallete+aph

    bfrgb=pack('<2sIHHI',b'BM',len(rgb)+0x36,0,0,0x36)
    birgb=pack('<IIiHH24s',0x28,w,height,1,24,b'\x00'*24)
    bmprgb=bfrgb+birgb+rgb
    return (bmprgb,bmpa)

def CombineDir(path1,path2):
    oris=os.listdir(path1)

    for pic in oris:
        if pic.rsplit('.',1)[0].endswith('$'):
            continue
        if os.path.isdir(path1+pic):
            if not os.path.exists(path2+pic):
                os.makedirs(path2+pic)
            inpics=os.listdir(path1+pic)
            if pic+'$' not in oris:
                nomsk=1
            else:
                nomsk=0
            for ipic in inpics:
                rgbf=open(path1+pic+'\\'+ipic,'rb')
                if not nomsk:
                    if not os.path.isfile(path1+pic+'$\\'+ipic):
                        continue
                    af=open(path1+pic+'$\\'+ipic,'rb')
                    newstm=CombinePic(rgbf.read(),af.read())
                    if newstm==0:
                        int3()
                    rgbf.close()
                    af.close()
                else:
                    newstm=rgbf.read()
                    rgbf.close()
                newf=open(path2+pic+'\\'+ipic,'wb')
                newf.write(newstm)
                newf.close()
        else:
            rgbf=open(path1+pic,'rb')
            fn,bn=pic.rsplit('.',1)
            if fn+'$.'+bn in oris:
                af=open(path1+fn+'$.'+bn,'rb')
                newstm=CombinePic(rgbf.read(),af.read())
                if newstm==0:
                    int3()
                rgbf.close()
                af.close()
            else:
                newstm=rgbf.read()
                rgbf.close()
                
            if newstm!=0:
                newf=open(path2+pic,'wb')
                newf.write(newstm)
                newf.close()

def SplitDir(path1,path2):
    oris=os.listdir(path1)

    for pic in oris:
        if os.path.isdir(os.path.join(path1,pic)):
            rgbdir=os.path.join(path2,pic)
            if not os.path.exists(rgbdir):
                os.makedirs(rgbdir)
            inpics=os.listdir(os.path.join(path1,pic))
            if len(inpics)==0:
                continue
            
            fs=open(os.path.join(path1,pic,inpics[0]),'rb')
            fs.seek(0x1c)
            if fs.read(2)==b'\x20\x00':
                msk=1
                mskdir=os.path.join(path2,pic+'$')
                if not os.path.exists(mskdir):
                    os.makedirs(mskdir)
            else:
                msk=0
            if msk==1:
                for ipi in inpics:
                    print('Processing '+os.path.join(pic,ipi))
                    rf=open(os.path.join(path1,pic,ipi),'rb')
                    rgbs,aphs=SplitPic(rf.read())
                    rf=open(os.path.join(rgbdir,ipi),'wb')
                    rf.write(rgbs)
                    rf=open(os.path.join(mskdir,ipi),'wb')
                    rf.write(aphs)
                    rf.close()
            else:
                for ipi in inpics:
                    print('Processing '+os.path.join(pic,ipi))
                    rf=open(os.path.join(path1,pic,ipi),'rb')
                    nf=open(os.path.join(rgbdir,ipi),'wb')
                    nf.write(rf.read())
                    nf.close()
                    rf.close()
                
        elif pic.lower().endswith('.bmp'):
            fs=open(os.path.join(path1,pic),'rb')
            stm=fs.read()
            print('Processing '+pic)
            ret=SplitPic(stm)
            if ret!=0:
                (rgbs,aphs)=ret
                rf=open(os.path.join(path2,pic),'wb')
                rf.write(rgbs)
                rf.close()
                fn,bn=pic.rsplit('.',1)
                af=open(os.path.join(path2,fn+'$.'+bn),'wb')
                af.write(aphs)
                af.close()
            else:
                rf=open(os.path.join(path2,pic),'wb')
                rf.write(stm)
                rf.close()

SplitDir(r'F:\Program Files\乙女が紡ぐ恋のキャンバス\Chipb\\',
         r'F:\Program Files\乙女が紡ぐ恋のキャンバス\newChip\\')
#CombineDir(r'F:\Program Files\乙女が紡ぐ恋のキャンバス\Chip\\',
#           r'F:\Program Files\乙女が紡ぐ恋のキャンバス\Chipb\\')
