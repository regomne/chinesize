import sys
import re
import struct

pat=re.compile(r'_[0-9a-zA-Z]+')

class CsxReader:
    parts={}
    constStr=[]
    def __init__(self,stm,stmLen):
        hdr=stm.read(8)
        if hdr!=b'Entis\x1a\0\0':
            raise Exception('not entis')
        stm.seek(0x10)
        magic,size=struct.unpack('32s8xQ',stm.read(0x30))
        if magic.strip(b'\0')!=b'Cotopha Image file':
            raise Exception('not image file')
        hdrsize=0x40
        if hdrsize+size>stmLen:
            raise Exception('head corrupted')
        p=0
        while p<size:
            id,idLen=struct.unpack('8sQ',stm.read(0x10))
            p+=0x10
            if p+idLen>size:
                raise Exception(id.decode()+' corrupted')
            self.parts[id.decode().strip()]=stm.read(idLen)
            p+=idLen

        if 'conststr' in self.parts:
            self.conststr=self._parseConstStr(self.parts['conststr'])

    def _readUint(self,stm,p):
        return stm[p]+stm[p+1]*(1<<8)+stm[p+2]*(1<<16)+stm[p+3]*(1<<24)
    
    def _parseConstStr(self,stm):
        strCnt,unk1,idxCnt=struct.unpack('III',stm[0:0xc])
        if unk1!=0:
            raise Exception('err unk1')
        p=idxCnt*4+0xc
        txt=['']
        while p<len(stm):
            cnt=self._readUint(stm,p)*2
            s=stm[p+4:p+4+cnt].decode('u16')
            s=s.replace('\r','\\x0d').replace('\n','\\x0a')
            txt.append(s)
            p+=4+cnt
            cnt=self._readUint(stm,p)
            p+=4+cnt*4
        if len(txt)!=strCnt:
            raise Exception('str cnt not match!')
        return txt
        
    def get(self,name):
        if name in self.parts:
            return self.parts[name]
        else:
            raise Exception('not have '+name)

    def getStr(self,idx):
        return self.conststr[idx]

    def parseImage(self):
        stm=self.get('image')
        i=0
        char=b'\x02\0\x06\0\0\0\x80'
        poss=[]
        errs=[]
        while i<len(stm):
            p=stm.find(char,i)
            if p==-1:
                break
            idx,nexti=struct.unpack('IB',stm[p+7:p+12])
            if idx>=len(self.conststr) or nexti>=0x1e:
                errs.append(p)
                i=p+7
            else:
                poss.append(idx)
                i=p+11
        return (poss,errs)

def isFull(s):
    if pat.search(s):
        return False
    for c in s:
        if ord(c)>=0x100:
            return True
    return False

def main():
    fs=open('script.csx','rb')
    fs.seek(0,2)
    flen=fs.tell()
    fs.seek(0)

    rdr=CsxReader(fs,flen)
    fs.close()

    poss,errs=rdr.parseImage()

    print('poss:',len(poss))

    if len(errs)!=0:
        print('err:',errs)
        
    posstr='var strIdx=['
    ntxt=[]
    for idx in poss:
        s=rdr.getStr(idx)
        if s!='' and isFull(s):
            posstr+='%d,'%idx
            ntxt.append(s)
    posstr+='];'
    fs=open('final.txt','wb')
    fs.write('\r\n'.join(ntxt).encode('u16'))
    fs.close()
    fs=open('strIdx.js','wb')
    fs.write(posstr.encode('utf-8'))
    fs.close()

main()
