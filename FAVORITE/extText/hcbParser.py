#encoding=utf-8
#py2.7

from struct import pack
from pdb import set_trace as int3

class HcbParser:
    def __init__(self,hcb):
        self.hcb=hcb
        self.text=[]
        self.sysfunc=[]
        self.func=[]
        self.codeBlockLen=0
        self.funcNameTable={}
        self.codepage='932'
        self.scan_flags=bytearray(len(hcb))
    def readstr(self):
        return str(self.hcb.read(self.hcb.readu8()).rstrip('\0'))

    mnemonics=[
        0,'InitStack','Call','CallSys','Ret','Ret1','Jmp','Jz',
        'Push0','Push1','PushI32','PushI16','PushI8','PushFloat','PushStr','PushGlobal',
        'PushStack','OP11','OP12','PushTop','PushTemp','PopGlobal','CpyStack','OP17',
        'OP18','Neg','Add','Sub','Mul','Div','Mod','Test',
        'CondAnd','CondOr','SetE','SetNE','SetG','SetLE','SetL','SetGE'
    ]

    inst_len=[
        0,2,4,2,0,0,4,4,
        0,0,4,2,1,4,0,2,
        1,2,1,0,0,2,1,2,
        1,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0
    ]

    def scanFunc(self,address):
        self.hcb.seek(address)
        if self.scan_flags[self.hcb.tell()]==1: return
        while 1:
            cur=self.hcb.tell()
            if cur>=self.codeBlockLen: break
            self.scan_flags[cur]=1
            inst=self.hcb.readu8()
            if inst>0x27 or inst==0: int3()
            if inst==0xe:
                self.readstr()
            elif inst>7 or inst<2 or inst==3:
                self.hcb.seek(self.inst_len[inst],1)
            elif inst==2:
                to_addr=self.hcb.readu32()
                self.func.append(to_addr)
                self.scanFunc(to_addr)
            elif inst==4 or inst==5:
                return
            elif inst==7:
                to_addr=self.hcb.readu32()
                self.scanFunc(to_addr)
            elif inst==6:
                to_addr=self.hcb/readu32()
                self.scanFunc(to_addr)
                return
    def scanFunc2(self):
        self.hcb.seek(4)
        cur=self.hcb.tell()
        j=0
        while cur<self.codeBlockLen:
            inst=self.hcb.readu8()
            if inst>0x27 or inst==0:
                print '%x'%self.hcb.tell()
                int3()
            if inst==1:
                addr=self.hcb.tell()-1
                if addr not in self.func:
                    self.func.append(addr)
                self.hcb.seek(2,1)
            elif inst==2:
                addr=self.hcb.readu32()
                if addr not in self.func:
                    self.func.append(addr)
            elif inst==0xe:
                self.readstr()
            else:
                self.hcb.seek(self.inst_len[inst],1)
            cur=self.hcb.tell()
            if cur>=j*0x50000:
                print '%x/%x'%(cur,self.codeBlockLen)
                j+=1
        self.func.append(self.codeBlockLen)
        self.func.sort()
    
    def SetFuncNameTable(self,tbl,code):
        self.funcNameTable=dict(tbl)
        self.codepage=code

    def Parse(self):
        self.codeBlockLen=self.hcb.readu32()
        self.hcb.seek(self.codeBlockLen)
        entry_point=self.hcb.readu32()
        self.hcb.seek(6,1)
        game_title=self.readstr()
        sysfunc_count=self.hcb.readu16()
        for i in range(sysfunc_count):
            self.hcb.seek(1,1)
            self.sysfunc.append(self.readstr())

        print 'Scanning Funcs...'
        self.scanFunc2()

        print 'Parsing the script...'
        self.hcb.seek(4)
        cur=self.hcb.tell()
        func_it=0
        j=0
        while cur<self.codeBlockLen:
            if cur==self.func[func_it]:
                self.text.append('')
                if cur in self.funcNameTable:
                    self.text.append('Func %s(%08X)'%(self.funcNameTable[cur].encode(self.codepage),cur))
                else:
                    self.text.append('Func #%04d(%08X)'%(func_it,cur))
                func_it+=1
            curline='%08X:  '%cur
            inst=self.hcb.readu8()
            if inst==3:
                num=self.hcb.readu16()
                if num>=len(self.sysfunc): int3()
                curline+=self.mnemonics[inst]+' '+self.sysfunc[num]
            elif inst==2:
                addr=self.hcb.readu32()
                if addr in self.funcNameTable:
                    curline+=self.mnemonics[inst]+' '+self.funcNameTable[addr].encode(self.codepage)
                else:
                    curline+=self.mnemonics[inst]+' '+str(self.func.index(addr))
            elif inst==0xe:
                curline+=(self.mnemonics[inst]+' '+self.readstr())
            else:
                ilen=self.inst_len[inst]
                if ilen==0:
                    opcode=''
                elif ilen==1:
                    opcode=' %X'%self.hcb.readu8()
                elif ilen==2:
                    opcode=' %X'%self.hcb.readu16()
                elif ilen==4:
                    opcode=' %X'%self.hcb.readu32()
                curline+=(self.mnemonics[inst]+opcode)
            self.text.append(curline)
            cur=self.hcb.tell()
            if cur>=j*0x50000:
                print '%x/%x'%(cur,self.codeBlockLen)
                j+=1
        return '\r\n'.join(self.text)

    def NeedExp(self,addr):
        if (addr>=0x3334a and addr<=0x3370b) or\
           (addr>=0x4326b and addr<=0x46a3c):
            return False
        return True
    def isFa(self,s):
        f=False
        for c in s:
            if ord(c)>=0x80:
                f=True
                break
        return f

    def ParseTxt(self,exBlocks,textStart):
        self.codeBlockLen=self.hcb.readu32()
        self.hcb.seek(self.codeBlockLen)
        entry_point=self.hcb.readu32()
        self.hcb.seek(6,1)
        game_title=self.readstr()
        sysfunc_count=self.hcb.readu16()
        for i in range(sysfunc_count):
            self.hcb.seek(1,1)
            self.sysfunc.append(self.readstr())

        print 'Scanning Funcs...'
        self.scanFunc2()

        print 'Parsing the script...'
        self.hcb.seek(4)
        idx=[]
        cur=self.hcb.tell()
        func_it=0
        j=0
        print 'Parsing 0000...'
        while 1:
            if cur==self.func[func_it]:
                for left,right in exBlocks:
                    if func_it==left:
                        func_it=right
                        cur=self.func[func_it]
                        self.hcb.seek(cur)
                if func_it==textStart:
                    break
                func_it+=1
            inst=self.hcb.readu8()
            if inst==0xe:
                ss=self.readstr()
                if self.isFa(ss) and self.NeedExp(cur):
                    self.text.append(ss)
                    idx.append(pack('I',cur+1))
            else:
                self.hcb.seek(self.inst_len[inst],1)
            cur=self.hcb.tell()
        fs=open('0000.txt','wb')
        fs.write('\r\n'.join(self.text).decode('932').encode('U16'))
        fs.close()
        fs=open('0000.idx','wb')
        fs.write(''.join(idx))
        fs.close()

        bopen=0
        txt=[]
        idx=[]
        while cur<self.codeBlockLen:
            if cur==self.func[func_it]:
                print 'Parsing %04d'%func_it
                if bopen:
                    tfile.write('\r\n'.join(txt).decode('932').encode('U16'))
                    idxfile.write(''.join(idx))
                    tfile.close()
                    idxfile.close()
                txt=[]
                idx=[]
                tfile=open('%04d'%func_it+'.txt','wb')
                idxfile=open('%04d'%func_it+'.idx','wb')
                func_it+=1
                bopen=1
            inst=self.hcb.readu8()
            if inst==0xe:
                txt.append(self.readstr())
                idx.append(pack('I',cur+1))
            elif inst==2:
                addr=self.hcb.readu32()
                if addr in self.funcNameTable:
                    txt.append((u'【'+self.funcNameTable[addr]+u'】').encode('932'))
                    idx.append(pack('I',cur+1))
            else:
                self.hcb.seek(self.inst_len[inst],1)
            cur=self.hcb.tell()
        tfile.write('\r\n'.join(txt).decode('932').encode('U16'))
        idxfile.write(''.join(idx))
        tfile.close()
        idxfile.close()
