import strfile
import os
import pdb
from struct import unpack
'''
struct NoriHeader{
	char szMagic[4]; //"NORI"
	int nVer; //0x10000
	int nNumberOfFunc; //struc1
	int nFuncTableLen;
	int nUnk;
	int nUnkLen;
	int nNumberOfFunc2; //struc2
	int nFunc2Len;
	int nCodeLen;
	int nStringLen;
};
'''

def decStr(astr):
    a=""
    for i in astr:
        a+=chr(i ^ 0x53)
    return a

def extBin(stm):
    fun1_len,fun2_len,fun3_len,code_len,res_len=unpack('12xI4xI4xIII',stm.read(0x28))
    stm.seek(fun1_len+fun2_len+fun3_len+0x28+4*2)
    res_off=stm.tell()+code_len+2
    newtxt=[]
    while stm.tell()<res_off-2:
        op, count, mne1, mne2=unpack('Hhhh',stm.read(8))
        if op==1:
            if count-mne1!=1: pdb.set_trace()
            stm.seek(8,1)
            line=[]
            for j in range(mne1):
                arg_type, len1, off=unpack('HHI',stm.read(8))
                if arg_type!=5: pdb.set_trace()
                line.append(decStr(stm[res_off+off:res_off+off+len1]))
            newtxt.append('\\n'.join(line))
        elif op==0x3c:
            if count-mne1!=1 or count<2: pdb.set_trace()
            stm.seek(8,1)
            arg_type, len1, off=unpack('HHI',stm.read(8))
            name=decStr(stm[res_off+off:res_off+off+len1])
            line=[]
            for j in range(mne1-1):
                arg_type, len1, off=unpack('HHI',stm.read(8))
                if arg_type!=5: pdb.set_trace()
                line.append(decStr(stm[res_off+off:res_off+off+len1]))
            newtxt.append(name+'@'+'\\n'.join(line))
        elif op==0x64:
            if count!=mne1: pdb.set_trace()
            line=[]
            for j in range(mne1):
                arg_type, len1, off=unpack('HHI',stm.read(8))
                if arg_type!=5: pdb.set_trace()
                newtxt.append(decStr(stm[res_off+off:res_off+off+len1]))
        else:
            if op not in range(2,9+1) and count>=0:
                stm.seek(count*8,1)
            elif count<0:
                stm.seek(8,1)

    return '\r\n'.join(newtxt)

if 'riotxt' not in os.listdir('.'):
    os.mkdir('riotxt')
for binf in os.listdir('rio'):
    if not binf.endswith('.bin'): continue
    fs=open('rio\\'+binf,'rb')
    stm=strfile.MyStr(fs.read())
    fs.close()
    txt=extBin(stm)
    if len(txt)!=0:
        fs=open('riotxt\\'+binf.replace('.bin','.txt'),'wb')
        fs.write(txt.encode('raw-unicode-escape').decode('932').encode('utf-16'))
        fs.close()
