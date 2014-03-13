
def c2i(c):
    s=0
    for b in c:
        s=(s<<8)+b
    return s

def Replace(stm,tbl):
    ns=[]
    for c in stm:
        if ord(c)<0x100:
            ns.append(c.encode('936'))
            continue
        nc=tbl[c2i(c.encode('936'))]
        if(nc!=b'\0\0'):
            ns.append(nc)
        else:
            raise Exception("no char!")
    return b''.join(ns)

def ReadTbl(stm):
    tbl=[0 for i in range(65536)]
    for i in range(65535):
        tbl[i]=stm[i*2:i*2+2]
    return tbl

tbl=ReadTbl(open('rvtbl.tbl','rb').read())
oriTxt=open(r'd:\temp\02.txt','rb').read().decode('936')
ns=Replace(oriTxt,tbl)
open(r'd:\temp\022.txt','wb').write(ns)
