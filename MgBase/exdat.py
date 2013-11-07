#encoding=utf-8
#py3.2
import bytefile
import struct
import os
import sys
#from pdb import set_trace as int3

#妖妖剑戟梦想

fname=r'rpack01.dat'
path=r'rpack01'

def int3():
    sys.exit()

def Dec(stm):
    for i in range(len(stm)):
        stm[i]^=0xa4

def ExtDat(fs,ndir):
    fs.seek(0)
    hdr=bytefile.ByteIO(fs.read(0x40))
    Dec(hdr)
    magic,val1,val2,f_count,index_len=struct.unpack('<8sIIII',hdr.read(0x18))
    if (magic!=b'mgbpack\0') or (val1!=1) or (val2!=0x40):
        print('Format Error!')
        int3()
    idx=bytefile.ByteIO(fs.read(index_len))
    Dec(idx)

    for i in range(f_count):
        slen=idx.readu32()
        name=idx.read(slen).decode('932')
        idx.seek(1,1)
        off,unk1,fsize,unk2=struct.unpack('<IIII',idx.read(16))
        #if not name.endswith('.dds'):
        #    continue
        #if (unk1!=0) or (unk2!=0):
        #    int3()

        print('Extracting '+name)
        fs.seek(off+index_len+0x40)
        stm=fs.read(fsize)
        fullname=os.path.join(ndir,name).replace('/','\\')
        fullpath=os.path.split(fullname)[0]
        if not os.path.exists(fullpath):
            os.makedirs(fullpath)
        nf=open(os.path.join(ndir,name),'wb')
        nf.write(stm)
        nf.close()
        ##if name.endswith('ms010.bnt'):
          ##  print('%x'%(off+0x40+index_len))
        
    print("Extract Complete!")

fs=open(fname,'rb')
ExtDat(fs,path)
