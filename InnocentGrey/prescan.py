#encoding=utf-8
#py3.2

import bytefile
import os
from struct import unpack
from pdb import set_trace as int3

path=r'K:\Program Files\クロウカシス\decscript'

spec_table=(
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,
    0,0,1,0,0,2,0,1,1,0,0,0,0,2,1,1, #2
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, #5
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, #8
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    )

def SScan(stm):
    while stm.tell()<len(stm):
        opcode,oplen,code1,code2=unpack('BBBB',stm.read(4))
        if opcode>0xa6:
            print('%x'%stm.tell())
            int3()
        if spec_table[opcode]==0:
            if opcode==0 or opcode==2 or opcode==0xf or opcode==0x10\
               or opcode==0x12 or opcode==0x3c or opcode==0x58:
                stm.seek(oplen-4+code2,1)
            else:
                stm.seek(oplen-4,1)
        elif spec_table[opcode]==3:
            int3()
            stm.seek(oplen-4+code1,1)
        elif spec_table[opcode]==1:
            strlen,=unpack('3xB',stm.read(4))
            stm.seek(oplen-8+strlen,1)
        elif spec_table[opcode]==2:
            strlen,=unpack('4xB3x',stm.read(8))
            stm.seek(oplen-12+strlen,1)
        else:
            int3()
        
for f in os.listdir(path):
    if not f.endswith('.s'): continue
    fs=open(path+os.sep+f,'rb')
    stm=bytefile.ByteIO(fs.read())
    print(f)
    SScan(stm)
    fs.close()
