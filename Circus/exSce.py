#encoding=utf-8
import os

def ExSce(stm):
    i=0
    txt=[]
    while i!=-1:
        i=stm.find(b'\x00\x45',i)
        if i==-1:
            break
        if stm[i-1]<10:
            p=stm.find(b'\0',i+2)
            if p==-1:
                raise Exception("sdf")
            try:
                txt.append(stm[i+2:p].decode('932'))
            except:
                print('excep: %x'%i)
                break
            i=p+1
        else:
            i+=2
    return txt

def PackSce(stm,txt):
    i=0
    prev=i
    j=0
    ns=bytearray()
    while True:
        i=stm.find(b'\x00\x45',i)
        if i==-1:
            ns+=stm[prev:]
            break
        if stm[i-1]<10:
            p=stm.find(b'\0',i+2)
            if p==-1:
                raise 'sdf'
            ns+=stm[prev:i+2]
            ns+=txt[j].encode('936')
            j+=1
            prev=p
            i=p+1
        else:
            i+=2
    return ns

fnames=['quiz_sel.mes','mj_sce.mes','mj_sce_column.mes','main_sce.mes']

# fs=open('mes01\\quiz_sel.mes','rb')
# txt=ExSce(fs.read())
# fs=open('a2.txt','wb')
# fs.write('\r\n'.join(txt).encode('u16'))
# fs.close()

# stm=open('mes01\\mj_sce.mes','rb').read()
# txt=open('a.txt','rb').read().decode('u16').replace('・','·').split('\r\n')

# ns=PackSce(stm,txt)
# fs=open('mj_sce.mes','wb')
# fs.write(ns)
# fs.close()

ispack=True

for fname in fnames:
    if not ispack:
        fs=open(os.path.join('mes01',fname),'rb')
        txt=ExSce(fs.read())
        fs=open(fname.replace('.mes','.txt'),'wb')
        fs.write('\r\n'.join(txt).encode('u16'))
        fs.close()
    else:
        stm=open(os.path.join('mes01',fname),'rb').read()
        txt=open(fname.replace('.mes','.txt'),'rb').read().decode('u16').replace('・','·').split('\r\n')
        ns=PackSce(stm,txt)
        fs=open(fname,'wb')
        fs.write(ns)
        fs.close()