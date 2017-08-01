import pdb

class ByteIO:
    "Operate String as a File."
    pData=0
    def __init__(self, Src):
        self.pData=0
        self.b=bytearray(Src)
    def __len__(self):
        return len(self.b)
    def __getitem__(self,i):
        return self.b[i]
    def __getslice__(self,i,j):
        return self.b[i:j]
    def __setitem__(self,i,y):
        self.b[i]=y
    def read(self, Bytes=-1):
        if Bytes<-1:
            return bytearray(b'')
        elif Bytes==-1:
            tData=self.b[self.pData:]
            self.pData=len(self.b)
        else:
            tData=self.b[self.pData:self.pData+Bytes]
            self.pData+=Bytes
        return tData
    def tell(self):
        return self.pData
    def seek(self, Offset, mode=0):
        if mode==1: Offset+=self.pData
        elif mode==2: Offset=len(self.b)-Offset
        if Offset<0:
            self.pData+=Offset
        elif Offset<len(self.b):
            self.pData=Offset
        else:
            self.pData=len(self.b)
    def readstr(self):
        p=self.b.find(b'\0',self.pData)
        if p==-1:
            p0=self.pData
            self.pData=len(self.b)
            return self.b[p0:]
        p0=self.pData
        self.pData=p+1
        return self.b[p0:p]
    def readu32(self):
        tData=self.b[self.pData:self.pData+4]
        self.pData+=4
        if len(tData)<4:
            return 0
        u32=(tData[3]<<24)+(tData[2]<<16)+(tData[1]<<8)+(tData[0])
        return u32
    def readu16(self):
        tData=self.b[self.pData:self.pData+2]
        self.pData+=2
        if len(tData)<2:
            return 0
        return (tData[1]<<8)+(tData[0])
    def readu8(self):
        if self.pData>=len(self.b):
            return 0
        self.pData+=1
        return self.b[self.pData-1]
    def readstrp(self,pos):
        p=self.b.find(b'\0',pos)
        if p==-1:
            return self.b[pos:]
        return self.b[pos:p]
    def readu32p(self,pos):
        if pos+3>=len(self.b):
            return 0
        return (self.b[pos+3]<<24)+(self.b[pos+2]<<16)+\
               (self.b[pos+1]<<8)+self.b[pos]
    def readu16p(self,pos):
        if pos+1>=len(self.b):
            return 0
        return (self.b[pos+1]<<8)+self.b[pos]
    def readu8p(self,pos):
        if pos>=len(self.b):
            return 0
        return self.b[pos]
