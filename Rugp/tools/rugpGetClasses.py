#encoding=utf-8
#用在immunity debugger里面

import pefile
import debugger
from immlib import *
from libhook import *
from wintypes2 import *

imm=Debugger()

logfname=ur'D:\Program Files\マブラヴオルタネイティヴクロニクルズ 01\rslt\ClassesWithoutCO.txt'

class retCOHook(LogBpHook):
    def __init__(self):
        LogBpHook.__init__(self)
        
    def run(self,regs):
        global imm
        name=self.descdict[regs['EIP']]
        eax=regs['EAX']
        if eax==0:
            imm.log('Create Object Failed! '+name)
            return
        vtbl=imm.readLong(eax)
        roAddr=imm.readLong(vtbl+0x14)
        imm.setLabel(roAddr,'RO!'+name)
        imm.setComment(roAddr,'RO!'+name)
        imm.setBreakpoint(roAddr)
        self.unhook2(name)
        imm.log('Break Finished: '+name)

class COHook(LogBpHook):
    def __init__(self):
        LogBpHook.__init__(self)
        

    def run(self,regs):
        global imm,grCOHook
        name=self.descdict[regs['EIP']]
        retAddr=imm.readLong(regs['ESP'])
        grCOHook.add2(name,retAddr)
        self.unhook2(name)
        
gCOHook=COHook()
grCOHook=retCOHook()

def main(args):
    fs=open(logfname,'w')
    regs=imm.getRegs()
    edi=regs['EDI']
    #dispatchObject
    while edi!=0:
        esi=imm.readLong(edi+0x28)
        while esi!=0:
            ecx=imm.readLong(esi)
            s=imm.readString(ecx)
            coRoutine=imm.readLong(esi+0xc)
            if coRoutine==0:
                fs.write(s+'\n')
                #imm.log(s)
                esi=imm.readLong(esi+0x14)
                continue
                imm.error('%x'%esi)
                debugger.pyreset()
                raise Exception("wocao!")
            imm.setLabel(coRoutine,'CO!'+s)
            imm.setComment(coRoutine,'CO!'+s)
            gCOHook.add2(s,coRoutine)
            esi=imm.readLong(esi+0x14)
        edi=imm.readLong(edi+0x3c)
    fs.close()
    #debugger.pyreset()
    return 'Success'
