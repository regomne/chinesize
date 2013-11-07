#py3.2
#encoding=utf-8

from struct import unpack,pack
import os
from os import path
import sys

from pdb import set_trace as int3

metaErrors=[]

G_strlen=13

def LzssPack(src):
    i=len(src)
    new=bytearray()
    cur=0
    while i>=8:
        new+=b'\xff'
        new+=src[cur:cur+8]
        cur+=8
        i-=8
    if i!=0:
        new+=bytes([(1<<i)-1])
        new+=src[cur:]
        new+=b'\0\0'
    else:
        new+=b'\0\0\0'
    return new

def swapBmp(dib,w,h,bitcount):
    if (bitcount!=24) and (bitcount!=32):
        print('Can only process 24 and 32 bit pic!.')
        int3()
    bc=int(bitcount/8)
    newdib=bytearray(h*w*bc)
    k=0
    if bc==3:
        for i in range(3):
            for j in range(h*w):
                newdib[k]=dib[j*3+i]
                k+=1
    else:
        for i in range(4):
            for j in range(h*w):
                newdib[k]=dib[j*4+(4-i-1)]
                k+=1
    return newdib

def reverseBmp(dib,w,h,b):
    newdib=bytearray()
    line_len=int(w*b/8)
    for i in range(h):
        newdib+=dib[(h-i-1)*line_len:(h-i)*line_len]
    return newdib

def PackWip(args):
    bmpfs,idxfname=args
    fs=open(idxfname,'rb')
    idxstm=bytearray(fs.read())
    fs.close()
    if len(idxstm)!=len(bmpfs)*0x18:
        print('文件个数不匹配！\n请确保%s所在文件夹中有从0000开始的所有文件'%bmpfs[0])
        sys.exit()

    g_bitcount=8
    wipbody=bytearray()
    for i in range(len(bmpfs)):
        print('Processing '+bmpfs[i])
        fs=open(bmpfs[i],'rb')
        bmp=fs.read()
        fs.close()

        iw,ih,ix,iy=unpack('IIII',idxstm[i*0x18:i*0x18+16])
        
        magic, bmp_size, off_bits=unpack('<HI4xI',bmp[0:14])
        hdr_size, width, height, planes, bit_count=unpack('<IIiHH',bmp[14:30])
        if (i!=0) and (g_bitcount!=bit_count):
            print('Bit count not match!')
            int3()
        if (bit_count!=8) and (bit_count!=24):
            print('Bit count must be 8 or 24')
            int3()
        g_bitcount=bit_count
        if height<0:
            rh=-height
        else:
            rh=height

        if (iw!=width) or (ih!=rh):
            print('Meta info not match!')
            erri=bmpfs[i]+' orignal width:%d height:%d\n\tcurrent width:%d height:%d'%(iw,ih,width,rh)
            metaErrors.append(erri)
            idxstm[i*0x18:i*0x18+4]=pack('I',width)
            idxstm[i*0x18+4:i*0x18+8]=pack('I',rh)
            #int3()
        
        if bit_count==8:
            pallete=bmp[off_bits-0x400:off_bits]
        else:
            pallete=b''
            
        dib=bmp[off_bits:]
        if len(dib)<width*rh*bit_count/8:
            int3()
        if height>0:
            dib=reverseBmp(dib,width,rh,bit_count)
        if bit_count!=8:
            dib=swapBmp(dib,width,rh,bit_count)
        compr=LzssPack(dib)
        idxstm[i*0x18+20:i*0x18+24]=pack('I',len(compr))
        
        wipbody+=pallete+compr
        
    wiphdr=pack('<4sHH',b'WIPF',len(bmpfs),g_bitcount)
    
    return wiphdr+idxstm+wipbody

def PackWap(args):
    bmpfs,idxfname=args
    fs=open(idxfname,'rb')
    idxstm=bytearray(fs.read())
    fs.close()
    if len(idxstm)!=len(bmpfs)*0x18:
        print('文件个数不匹配！\n请确保%s所在文件夹中有从0000开始的所有文件'%bmpfs[0])
        sys.exit()

    wapbody=bytearray()
    for i in range(len(bmpfs)):
        print('Processing '+bmpfs[i])
        fs=open(bmpfs[i],'rb')
        bmp=fs.read()
        fs.close()

        iw,ih,ix,iy=unpack('IIII',idxstm[i*0x18:i*0x18+16])
        
        magic, bmp_size, off_bits=unpack('<HI4xI',bmp[0:14])
        hdr_size, width, height, planes, bit_count=unpack('<IIiHH',bmp[14:30])
        if bit_count!=32:
            print('Bit count not match!')
            int3()
        if height<0:
            rh=-height
        else:
            rh=height

        if (iw!=width) or (ih!=rh):
            print('Meta info not match!')
            idxstm[i*0x18:i*0x18+4]=pack('I',width)
            idxstm[i*0x18+4:i*0x18+8]=pack('I',rh)
            #int3()
        
        dib=bmp[off_bits:]
        if len(dib)<width*rh*bit_count/8:
            int3()
        if height>0:
            dib=reverseBmp(dib,width,rh,bit_count)
        dib=swapBmp(dib,width,rh,bit_count)
        compr=LzssPack(dib)
        idxstm[i*0x18+20:i*0x18+24]=pack('I',len(compr))
        
        wapbody+=compr
        
    waphdr=pack('<4sHH',b'WAPF',len(bmpfs),0)
    
    return waphdr+idxstm+wapbody

def PackWsc(fname):
    print('Processing '+fname)
    fs=open(fname,'rb')
    stm=fs.read()
    fs.close()
    return stm

def PackArc(fs,types,data,packf,names):
    if (len(types)!=len(data)) or (len(types)!=len(names))\
        or (len(types)!=len(packf)):
        int3()
    itype=len(types)
    hdr=bytearray(pack('I',itype))
    curpos=4+itype*12
    for i in range(itype):
        hdr+=pack('4sII',types[i],len(data[i]),curpos)
        curpos+=len(data[i])*(G_strlen+8)

    data_start=curpos
    fs.seek(curpos)
    entry=bytearray()
    for i in range(itype):
        for j in range(len(data[i])):
            bf=packf[i](data[i][j])
            entry+=names[i][j].encode('932').ljust(G_strlen,b'\0')
            entry+=pack('II',len(bf),curpos)
            fs.write(bf)
            curpos+=len(bf)

    fs.seek(0)
    if len(hdr)+len(entry)!=data_start:
        int3()
    fs.write(hdr)
    fs.write(entry)

def PackPics(dirname,fname):
    dirs=os.listdir(dirname)
    msks=[]
    wips=[]
    waps=[]
    wipnames=[]
    msknames=[]
    wapnames=[]
    for di in dirs:
        if path.isdir(path.join(dirname,di)):
            #int3()
            bmpfs=[path.join(dirname,di,f) \
                  for f in os.listdir(path.join(dirname,di))]
            idxname=path.join(dirname,di+'.idx')
            if di[-1]=='$':
                msks.append((bmpfs,idxname))
                msknames.append(di[0:-1].upper())
            elif di[-1]=='~':
                waps.append((bmpfs,idxname))
                wapnames.append(di[0:-1].upper())
            else:
                wips.append((bmpfs,idxname))
                wipnames.append(di.upper())
        else:
            if not di.endswith('.bmp'):
                continue
            bmpfs=[path.join(dirname,di)]
            resname=di.rpartition('.')[0]
            idxname=path.join(dirname,resname+'.idx')
            if resname[-1]=='$':
                msks.append((bmpfs,idxname))
                msknames.append(resname[0:-1].upper())
            elif resname[-1]=='~':
                waps.append((bmpfs,idxname))
                wapnames.append(resname[0:-1].upper())
            else:
                wips.append((bmpfs,idxname))
                wipnames.append(resname.upper())
    fs=open(fname,'wb')
    PackArc(fs,[b'MSK\0',b'WIP\0',b'WAP\0'],[msks,wips,waps],\
            [PackWip,PackWip,PackWap],[msknames,wipnames,wapnames])
    fs.close()

def PackScript(dirname,fname):
    dirs=os.listdir(dirname)
    wscs=[]
    wscnames=[]

    for di in dirs:
        if not di.lower().endswith('.wsc'):
            continue
        wscs.append(os.path.join(dirname,di))
        wscnames.append(di.rpartition('.')[0])
    fs=open(fname,'wb')
    PackArc(fs,[b'WSC\0'],[wscs],[PackWsc],[wscnames])
    fs.close()

#main
PackPics(r'F:\Program Files\乙女が紡ぐ恋のキャンバス\newchip',
         r'chip_ch.arc')
#PackScript(r'F:\Program Files\Princess Waltz\0rio','rio1.arc')
