#encoding=utf-8
#用在immunity debugger里面

import pefile
import debugger
from immlib import *
from libhook import *
from wintypes2 import *

imm=Debugger()

class vmBp(LogBpHook):
    def __init__(self,name):
        LogBpHook.__init__(self)
        self.cname=name
    def run(self,regs):
        name=getVMMsg(regs)
        #imm.log(name)
        if name==self.cname:
            imm.pause()
            imm.log(str(regs['EDX']))
            self.unhook2('desc')

def CalcStringHash(s):
    h=0
    bs=s.encode('932')
    for c in bs:
        h=(ord(c)+(h<<5)+h)&0xffffffff
    return h

def FindOMClass(hdr,om):
    h=CalcStringHash(om)
    esi=imm.readLong(hdr+(h%0x11)*4)
    while esi!=0:
        ss=imm.readString(imm.readLong(esi))
        if om==ss.partition('/')[0]:
            return esi
        esi=imm.readLong(esi+0xc)
    return 0

def FindAllOM(hdr):
    tbl={}
    p=hdr
    global instTbl
    for i in range(0x11):
        esi=imm.readLong(p)
        while esi!=0:
            ss=imm.readString(imm.readLong(esi))
            om=ss.partition('/')[0]
            o=imm.readLong(esi+4)
            if o==0:
                tbl[om]='None'
                esi=imm.readLong(esi+0xc)
                continue
            s=[]
            imm.log(om.decode('932'))
            while imm.readLong(o+8)!=0xffff0000:
                ii=imm.readLong(o+4)
                inst=imm.readLong(ii+8)
                if inst not in instTbl:
                    addr=imm.readLong(o+4)
                    s.append('%x'%addr)
                    break
                s.append(instTbl[inst])
                o+=0xc
            tbl[om]=' '.join(s)
            esi=imm.readLong(esi+0xc)
        p+=4
    return tbl

instTbl={0x91000000:'b',
         0x70000000:'list',
         0x70000001:'list',
         0x90000000:'str',
         0x90500000:'d',
         0x92000000:'b',
         0x93000000:'b',
         0x94000000:'w',
         0x95000000:'w',
         0x96000000:'d',
         0x97000000:'d',
         0x98000000:'d',
         0x9a000000:'d',
         0x9b000000:'sub1',
         0x9c000000:'d', #????
         0x9e000000:'err',
         0xe0000002:'list',
         0xe0000008:'list',
         0xe0000004:'list',
         0xe0000006:'list',
         0xe0000011:'list',
         0xe0000001:'list',
         0xe0000000:'list',
         0xe0000010:'list',
         }
def ParseOM(omc):
    esi=imm.readLong(omc+4)
    global instTbl
    s=[]
    while imm.readLong(esi+0x8)!=0xffff0000:
        inst=imm.readLong(imm.readLong(esi+4)+8)
        if inst not in instTbl:
            addr=imm.readLong(esi+4)
            s.append('%x'%addr)
            imm.gotoDumpWindow(esi)
            return ' '.join(s)
        s.append(instTbl[inst])
        esi+=0xc
    return ' '.join(s)

def getVMMsg(regs):
    vtbl=imm.readLong(regs['ESI'])
    faddr=imm.readLong(vtbl)
    insts=imm.readMemory(faddr,6)
    if (insts[0]=='\xb8') and (insts[5]=='\xc3'):
        crc=imm.readLong(faddr+1)
        nameAddr=imm.readLong(crc)
        name=imm.readString(nameAddr)
        return name
    else:
        return "error"

def listCache(arr):
    cnt=imm.readLong(arr+8)
    p=imm.readLong(arr+4)
    imm.log('Count: %d'%cnt)
    for i in range(cnt):
        addr=imm.readLong(p)
        if addr==0:
            imm.log('NULL',i)
        else:
            s=imm.readLong(addr)
            if s==0:
                imm.log('NULLStr',i)
            else:
                imm.log(imm.readString(s),i)
        p+=4

def listType(head):
    esi=imm.readLong(head)
    while esi!=0:
        addr=imm.readLong(esi)
        name=imm.readString(addr).decode('932').encode('936')
        #length=imm.readLong(imm.readLong(imm.readLong(esi+0x48)+0xc)+4)
        s='%s: %d'%(name,imm.readLong(esi+4))
        imm.log(s)
        esi=imm.readLong(esi+0x30)

#OMTableAddr=0x69a40
OMTableAddr=0x60480

def main(args):
    if len(args)==0:
        return 'args error'
    if args[0]=='getvmmsg':
        return getVMMsg(imm.getRegs())
    elif args[0]=='bvm':
        m=imm.getModule('Vm60.dll')
        vm=vmBp(args[1])
        vm.add2('desc',m.baseaddress+0x29771)
        return 'done'
    elif args[0]=='findOM':
        m=imm.getModule('UnivUI.dll')
        ret=FindOMClass(m.baseaddress+OMTableAddr,args[1])
        if ret==0:
            return 'not found'
        else:
            imm.gotoDumpWindow(ret)
            return 'done'
    elif args[0]=='findAllOM':
        m=imm.getModule('UnivUI.dll')
        tbl=FindAllOM(m.baseaddress+OMTableAddr)
        fs=open('a.txt','w')
        fs.write(repr(tbl))
        fs.close()
        return 'done'
        
    elif args[0]=='listTypes':
        m=imm.getModule('UnivUI.dll')
        listType(m.baseaddress+0x69a3c)
        return 'done'
    elif args[0]=='listCache':
        listCache(int(args[1],16))
        imm.createLogWindow()
        return 'done'
    else:
        return 'args error'
