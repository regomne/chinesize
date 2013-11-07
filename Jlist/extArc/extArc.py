#encoding=utf-8
#py3.2

from struct import unpack
import os
import pdb

Ext_idx=1

def GetIdx(stm):
    magic,count,bitcount=unpack('<4sHH',stm[0:8])
    return stm[8:8+count*0x18]

def extract(fname,pathname):

    fs=open(fname,'rb')

    type_count,=unpack('I',fs.read(4))
    types=[]
    for i in range(type_count):
        types.append(unpack('4sII',fs.read(12)))

    for (tname, count, toff) in types:
        fs.seek(toff)
        extname=tname.rstrip(b'\0').decode('932').lower()
        if extname=='wip':
            extname='.idx'
        elif extname=='msk':
            extname='$.idx'
        else:
            extname='.'+extname
        for j in range(count):
            arcname, size, offset=unpack('<13sII',fs.read(21))
            cur=fs.tell()
            newf=open(os.path.join(pathname,
                      arcname.rstrip(b'\0').decode('932')+extname),'wb')
            fs.seek(offset)
            if Ext_idx:
                idx=GetIdx(fs.read(size))
                newf.write(idx)
            else:
                newf.write(fs.read(size))
            newf.close()
            fs.seek(cur)


extract(r'F:\Program Files\乙女が紡ぐ恋のキャンバス\Chip.arc',\
        'chip')
