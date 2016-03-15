import bntParser
import bytefile
import os

path=r'E:\Program Files\妖々剣戟夢想\resource\rpack01'
path2=r'E:\Program Files\妖々剣戟夢想\resource\rpack01t'

pos=len(path)+1
for root,dirs,files in os.walk(path):
    for f in files:
        if not f.endswith('.bnt'):
            continue
        if not os.path.exists(os.path.join(path2,root[pos:])):
            os.makedirs(os.path.join(path2,root[pos:]))
        fs=open(os.path.join(root,f),'rb')
        stm=bytefile.ByteIO(fs.read())
        parser=bntParser.BntParser(stm)
        lns=parser.ExtTxt()
        pure=[]
        for l in lns:
            l=l.strip('\t')
            half=True
            for c in l:
                if ord(c)>=0x80:
                    half=False
                    break
            if not half:
                pure.append(l)
        fs=open(os.path.join(path2,root[pos:],f.replace('.bnt','.txt')),'wb')
        fs.write('\r\n'.join(pure).encode('U16'))
        fs.close()
        
'''
fs=open('ms010.bnt','rb')
stm=bytefile.ByteIO(fs.read())
parser=bntParser.BntParser(stm)
lns=parser.ExtTxt()
pure=[]
for l in lns:
    if l.strip('\t').startswith("'"):
        l=l.strip("\t'")
        half=True
        for c in l:
            if ord(c)>=0x80:
                half=False
                break
        if not half:
            pure.append(l)
##fs=open('1.txt','wb')
##fs.write('\r\n'.join(lns).encode('U16'))
##fs.close()
fs=open('2.txt','wb')
fs.write('\r\n'.join(pure).encode('U16'))
fs.close()
'''
