#encoding=utf-8

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
        s=s.encode('936')
        if len(s)>length:
            print(i,' too long')
            hasErr=True
            continue
        stm[off:off+len(s)]=s
        stm[off+len(s)]=0
    return hasErr

seg=[(0x90468,0x162a80),(0x7aef8,0x7bd84),(0x7bdac,0x7e084),(0x7e528,0x7e798),(0x7ea34,0x80a48)]

fs=open('keiten.exe','rb')
stm=fs.read()
for i in range(len(seg)):
    txt=getTxt(stm,seg[i][0],seg[i][1])
    fs=open('%d.txt'%i,'wb')
    fs.write(MakeRepr(txt).encode('u16'))
    fs.close()
