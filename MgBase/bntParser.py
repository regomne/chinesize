import struct
from pdb import set_trace as int3

class BntParser:
    tabc=0
    txt=[]
    txttbl=[]
    startp=[]
    curoff=[]
    stm=0
    
    def __init__(self,oss):
        self.txt=[]
        self.txttbl=[]
        self.startp=[]
        self.curoff=[]
        self.stm=oss
    def AddText(self,s):
        off=self.curoff[-1]
        #self.txt.append(('%04x:'%off)+'\t'*self.tabc+s)
        self.txttbl.append(off)
        self.txt.append('\t'*self.tabc+s)
    def Repre(self,o):
        if type(o)==str:
            return o
        elif type(o)==int:
            return '%x'%o
        else:
            return repr(o)
    def GetVal(self):
        self.curoff[-1]=self.stm.tell()
        t=self.stm.readu32()
        if t==0x8000010:
            l=self.stm.readu32()
            s=self.stm.read(l).decode('932').replace('\n','\\n')
            '''half=True
            for c in s:
                if c>0x80:
                    half=False
                    break
            if not half:
                Txt.append(s.decode('932'))'''
            return s
        elif t==0x5000002:
            return self.stm.readu32()
        elif t==0x5000004:
            return self.stm.readu32()
        elif t==0x1000001:
            return ''
            
    def CheckMagic(self):
        if self.stm.readu32()!=0x50415254:
            int3()

    def GetPart(self):
        self.startp.append(self.stm.tell())
        self.curoff.append(0)
        self.CheckMagic()
        self.AddText('')
        src_name=self.GetVal()
        func_name=self.GetVal()
        self.AddText(self.Repre(func_name)+'{')
        self.tabc+=1

        self.CheckMagic()
        counts=struct.unpack('I'*8,self.stm.read(32))
        
        self.CheckMagic()
        self.AddText('')
        self.AddText('Val1:')
        for i in range(counts[0]):
            self.AddText(self.Repre(self.GetVal()))

        self.CheckMagic()
        self.AddText('')
        self.AddText('Val2:')
        for i in range(counts[1]):
            self.AddText(self.Repre(self.GetVal()))

        self.CheckMagic()
        self.AddText('')
        self.AddText('Val3:')
        for i in range(counts[2]):
            self.AddText(self.Repre(self.stm.readu32()))
            self.AddText(self.Repre(self.GetVal()))
            self.AddText(self.Repre(self.GetVal()))

        self.CheckMagic()
        self.AddText('')
        self.AddText('Val4:')
        for i in range(counts[3]):
            self.AddText(self.Repre(self.GetVal()))
            self.AddText(self.Repre(self.stm.readu32()))
            self.AddText(self.Repre(self.stm.readu32()))
            self.AddText(self.Repre(self.stm.readu32()))

        self.CheckMagic()
        self.AddText('')
        self.AddText('Val5:')
        for i in range(counts[4]):
            self.AddText(self.Repre(self.stm.readu32())+\
                         ','+self.Repre(self.stm.readu32()))

        self.CheckMagic()
        self.AddText('')
        self.AddText('Val6:')
        for i in range(counts[5]):
            self.AddText(self.Repre(self.stm.readu32()))

        self.CheckMagic()
        self.AddText('')
        self.AddText('Val7:')
        for i in range(counts[6]):
            self.AddText(self.Repre(self.stm.readu32())+\
                         ','+self.Repre(self.stm.readu32()))

        self.CheckMagic()
        self.AddText('')
        self.AddText('SubFunction:')
        for i in range(counts[7]):
            self.GetPart()

        self.stm.seek(6,1)
        self.tabc-=1
        self.AddText('}')
        self.startp.pop()
        self.curoff.pop()

    def ExtTxt(self):
        self.txt=[]
        if self.stm.read(2)!=b'\xfa\xfa':
            int3()
        if self.stm.readu32()!=0x53514952:
            int3()
        if self.stm.readu32()!=1:
            int3()
        self.GetPart()
        if self.stm.read(4)!=b'LIAT':
            int3()
        return self.txt

    def ImpTxt(self,lines):
        if len(lines)!=len(self.txt):
            print("Lines not match!")
            int3()
        #int3()
        for i in range(len(lines)):
            if lines[i]==self.txt[i]:
                continue
            off=self.txttbl[i]
            self.stm.seek(off+4)
            slen=self.stm.readu32()
            nstr=lines[i].replace('\\n','\n').encode('936','replace')
            self.stm[off+4:off+8+slen]=struct.pack('I',len(nstr))+nstr
            
            j=i+1
            dist=len(nstr)-slen
            while j<len(self.txttbl):
                self.txttbl[j]+=dist
                j+=1
        return self.stm
