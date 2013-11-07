#encoding=gbk
import strfile
import os
import pdb
from pdb import set_trace as int3

#提取旧版的bgi引擎脚本文本，字符串与指令混合在脚本的代码段。
def extbgi(stm):
    res_off_min=stm.rfind(b'\x00\x00\x00')
    newtxt=[]
    while stm.tell()<res_off_min+5:
        code=stm.read(1)
        if code==b'\x14' and stm[stm.tell()]==0:
            stm.seek(1,1)
            name=stm.readstr()
            if stm[stm.tell()]==0 or stm[stm.tell()]<0x80: continue
            stm.seek(stm.find((b'\x10\x00\x00\x00\x00\x00\x01\x00\x00\x00'),stm.tell())+10)
            off=stm.readu32()
            if off<=res_off_min: int3()
            cur=stm.tell()
            stm.seek(off)
            newtxt.append(stm.readstr())
            newtxt.append(name)
            stm.seek(cur)
        elif code==b'\x10' and stm[stm.tell():stm.tell()+9]==(b'\x00'*5+b'\x01\x00\x00\x00'):
            stm.seek(9,1)
            off=stm.readu32()
            if off<=res_off_min:
                stm.seek(-4,1)
                continue
            cur=stm.tell()
            stm.seek(off)
            if stm[stm.tell()]!=0:
                newtxt.append(stm.readstr())
            stm.seek(cur)
        elif code==b'\xb0' and stm[stm.tell()]==0:
            stm.seek(1,1)
            count=stm.readu32()
            if count>5 or count==0:
                stm.seek(-4,1)
                continue
            for i in range(count):
                newtxt.append(stm.readstr())
    return b'\r\n'.join(newtxt)

for fi in os.listdir('arc00'):
    fs=open('arc00\\'+fi,'rb')
    stm=strfile.MyStr(fs.read())
    fs.close()
    txt=extbgi(stm)
    fs=open('txt\\'+fi+'.txt','wb')
    fs.write(txt)
    fs.close()

