#encoding=utf-8
import sys
import struct

def IsFullSjisString(stm):
    i=0
    full=False
    while i<len(stm):
        if (stm[i]<0x20 and stm[i]!=0xd and stm[i]!=0xa) or\
            (stm[i]>=0xa0 and stm[i]<0xe0):
            return False
        elif stm[i]<0x80:
            i+=1
        elif (i<len(stm)-1) and (stm[i+1]>=0x20):
            full=True
            i+=2
        else:
            return False
    return full

def getTxt(stm,begin,end):
    i=begin
    txt=[]
    while i<end:
        if stm[i]==0:
            i+=1
        else:
            p=stm.find(b'\0',i)
            if p==-1 or p>end:
                p=end
            if IsFullSjisString(stm[i:p]):
                try:
                    txt.append((stm[i:p].decode('932'),i,p-i))
                except:
                    print('%x'%i)
                    i=p+1
                    continue
            i=p+1
    return txt

def MakeRepr(txt):
    s=[]
    for t in txt:
        s.append(repr(t).replace('\\u3000','　'))
    return '[\r\n'+', \r\n'.join(s)+'\r\n]'

def Replace(txts,stm):
    i=0
    hasErr=False
    for s,off,length in txts:
        i+=1
        ns=s.replace('\u30fb','·')
        ns=ns.encode('936')
##        if len(s)>length:
##            print(i,' too long')
##            hasErr=True
##            continue
        stm[off:off+len(ns)]=ns
        stm[off+len(ns)]=0
    return hasErr

def ext_sp1(stm, begin, end):
    txt=[]
    i=begin
    while i<end:
        if struct.unpack('I',stm[i:i+4])[0]!=0:
            p=stm.find(b'\0',i+8)
            if p==-1 or p>end:
                p=end
            try:
                txt.append((stm[i+8:p].decode("932"),i+8,p-(i+8)))
            except:
                print('%x'%i)
                i=p+1
                continue
        else:
            break
        i+=0x10c
    return txt

Pack = 1
if Pack==0:

    seg=[(0x7dfe0,0x9ac10),(0x144498,0x15f280)]
    spseg=(0x13c7fc,0x140210)

    fs=open(r'd:\galgame\fortissimo FA\fortissimo_fa2.exe','rb')
    stm=fs.read()
    alltxt=[]
    for i in range(len(seg)):
        alltxt+=getTxt(stm,seg[i][0],seg[i][1])

    alltxt+=ext_sp1(stm, spseg[0], spseg[1])

    fs=open('exe.txt','wb')
    fs.write(MakeRepr(alltxt).encode('u16'))
    fs.close()
else:
    if len(sys.argv)!=4:
        print('usage: %s <exe> <txt> <new_exe>'%sys.argv[0])
        sys.exit(0)
    print('opening exe...')
    fs=open(sys.argv[1],'rb')
    stm = bytearray(fs.read())
    fs.close()
    print('reading text...')
    fs=open(sys.argv[2],'rb')
    text = fs.read().decode('u16')
    fs.close()
    txts = eval(text)
    print('Replacing...')
    Replace(txts,stm)
    print('Writing...')
    fs=open(sys.argv[3],'wb')
    fs.write(stm)
    fs.close()
    print('complete')
    
