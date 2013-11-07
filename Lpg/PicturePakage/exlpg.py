#py3.2

import bytefile
import struct
import os

path1='lpg'
path2='bmp'

def ExLpg(stm):
    unk1,bitcount,unk2,unk3,width,height,length=struct.unpack("IIiIIII",stm.read(28))
    if unk1!=1 or unk2!=-1 or unk3!=0 or bitcount!=32:
        print('Format error!')
        return ''
    pallete=[stm.read(4) for i in range(0x100)]
    newdib=bytearray()
    i=0
    while i<(int)(length/2):
        idx=stm.readu8()
        newdib+=pallete[idx][0:3]
        newdib.append(stm.readu8())
        i+=1

    bmpfhdr=struct.pack('<2sIII',b'BM',length*2+0x36,0,0x36)
    bmpihdr=struct.pack('IIIHHIIIIII',0x28,width,height,1,bitcount,\
                        0,0,0,0,0,0)
    return bmpfhdr+bmpihdr+newdib


for f in os.listdir(path1):
    fs=open(os.path.join(path1,f),'rb')
    stm=bytefile.ByteIO(fs.read())
    print('Processing ',f)
    newbmp=ExLpg(stm)
    fs=open(os.path.join(path2,f.replace('.lpg','.bmp')),'wb')
    fs.write(newbmp)
    fs.close()
    
