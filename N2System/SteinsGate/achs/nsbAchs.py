#encoding=utf-8
import strfile
import os
from struct import unpack,pack

def readarg(nsb):
    return nsb.read(nsb.readu32())

def ExtTips(nsb):
    stroff=[]
    strs=[]
    e_stroff=[]
    e_strs=[]
    while(nsb.tell()<len(nsb)-1):
        sno, opcode, argc=unpack('IHH',nsb.read(8))
        if opcode==0xd0:
            readarg(nsb)
            stroff.append(nsb.tell())
            strs.append(readarg(nsb))
            if len(strs[-1])>=6 and strs[-1][0:6]=='\x81\x48\x81\x48\x81\x48':
                e_stroff.append(stroff[-1])
                e_strs.append(strs[-1])
            elif len(strs[-1])>=16 and strs[-1][0:4]=='\x89\xf0\x8f\x9c':
                e_stroff.append(stroff[-1])
                e_strs.append(strs[-1])
        elif opcode==0x8e:
            stroff=[]
            strs=[]
        elif opcode==0x72 and argc==7:
            e_stroff+=[stroff[-3],stroff[-1]]
            e_strs+=[strs[-3],strs[-1]]
            for i in range(7):
                readarg(nsb)
        else:
            for i in range(argc): readarg(nsb)
    return (e_stroff,e_strs)

#(list<int>, list<str>, MyStr, MyStr)
def PackTips(idx, strs, nsb, maps):
    maps_idx=[]
    while(maps.tell()<len(maps)-1):
        off, len1=unpack('IH',maps.read(6))
        maps.read(len1)
        maps_idx.append(off)

    newNsb=[]
    lastpos=0
    for i in range(len(idx)):
        newNsb.append(nsb[lastpos:idx[i]])
        newNsb.append(pack('I',len(strs[i])))
        newNsb.append(strs[i])
        oldlen,=unpack('I',nsb[idx[i]:idx[i]+4])
        lastpos=idx[i]+4+oldlen
        dist=len(strs[i])-oldlen
        if dist!=0:
            for j in range(len(maps_idx)):
                if maps_idx[j]>idx[i]:
                    maps_idx[j]=maps_idx[j]+dist
    newNsb.append(nsb[lastpos:])

    newMaps=[]
    maps.seek(0)
    for midx in maps_idx:
        len1,=unpack('4xH',maps.read(6))
        newMaps.append(pack('IH',midx,len1))
        newMaps.append(maps.read(len1))
    return (''.join(newNsb),''.join(newMaps))

file_name='extra_achievements.nsb'
if 0:
    fs=open(file_name,'rb')
    stm=strfile.MyStr(fs.read())
    fs.close()
    ext=ExtTips(stm)
    index_data=''.join([pack('I',i) for i in ext[0]])
    str_data='\r\n'.join(ext[1])

    fs=open(file_name.replace('.nsb','.idx'),'wb')
    fs.write(index_data)
    fs.close()
    fs=open(file_name.replace('.nsb','.txt'),'wb')
    fs.write(str_data)
    fs.close()
else:
    fs=open(file_name,'rb')
    nsb=strfile.MyStr(fs.read())
    fs.close()
    fs=open(file_name.replace('.nsb','.txt'),'rb')
    strs=fs.read().replace('\\n','\n').split('\r\n')
    fs.close()
    fs=open(file_name.replace('.nsb','.idx'),'rb')
    idxf=strfile.MyStr(fs.read())
    fs.close()
    idx=[idxf.readu32() for i in range(len(idxf)/4)]
    if len(idx)!=len(strs):
        print u'行数不匹配！'
        a=a+1
    fs=open(file_name.replace('.nsb','.map'),'rb')
    maps=strfile.MyStr(fs.read())
    fs.close()

    newNsb, newMaps=PackTips(idx,strs,nsb,maps)
    fs=open(file_name.replace('.nsb','1.nsb'),'wb')
    fs.write(newNsb)
    fs.close()
    fs=open(file_name.replace('.nsb','1.map'),'wb')
    fs.write(newMaps)
    fs.close()

