from struct import unpack,pack
from pdb import set_trace as int3
import os

def u32(fs):
    bs=fs.read(4)
    return bs[3]*0x1000000+(bs[2]<<16)+(bs[1]<<8)+bs[0]

def pstr(fs):
    slen=u32(fs)
    return fs.read(slen).rstrip(b'\0').decode('932').replace('\n','\\n')

def ReadSc(fname):
    fs=open(fname,'rb')
    hdr=unpack('IIII',fs.read(0x10))

    #int3()
    Strings=[]
    for i in range(hdr[3]):
        Strings.append(pstr(fs))

    funcCnt=u32(fs)
    Funcs=[]
    for i in range(funcCnt):
        ent={}
        ent['start']=u32(fs)
        ent['name']=pstr(fs)
        ent['unk1']=u32(fs)
        ent['unk2']=u32(fs)
        Funcs.append(ent)

    offcnt=u32(fs)
    Offsets=[]
    for i in range(offcnt):
        Offsets.append(u32(fs))

    globalcnt=u32(fs)
    GlobalVars=[]
    for i in range(globalcnt):
        var={}
        var['type']=fs.read(1)[0]
        var['id']=u32(fs)
        var['name']=pstr(fs)
        var['unk']=u32(fs)
        GlobalVars.append(var)

    fs.read(4)
    codeCnt=u32(fs)
    Codes=[]
    for i in range(codeCnt):
        Codes.append(unpack('i',fs.read(4))[0])
    fs.close()
    return {'Strings':Strings,
            'Funcs':Funcs,
            'Offsets':Offsets,
            'GlobalVars':GlobalVars,
            'Codes':Codes}
mne=[
    	"End","","push","push","pushGlobal","","inst6","Add",
	"Sub","Imu","Div","Mod","And","Or","Xor","IsZero",
	"Neg","IsEqua","IsNotEqua","IsGreater","IsGE","IsLittle","IsLE","CondAnd",
	"CondOr","pop","jump","jne","je","TextNumber","Line:","Unk:",
        "Jx1","Jx2"
        ]
mneType=[
    0,1,4,4,2,3,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,4,4,4,4,4,4,
    4,0,
    ]

def isAllHalf(s):
    for c in s:
        if ord(c)>=0x100:
            return False
    return True

def ParseSc(sc):
    text=[]
    puretxt={}
    i=0
    Codes=sc['Codes']
    GlobalVars=sc['GlobalVars']
    Funcs=sorted(sc['Funcs'],key=lambda func: func['start'])
    Strings=sc['Strings']
    fi=0
    ltxt=[]
    lidx=[]
    while i<len(Codes):
        op=Codes[i]
        if op>=len(mneType):
            raise Exception("%x exceed"%op)

        if fi<len(Funcs) and i==Funcs[fi]['start']:
            fun=Funcs[fi]
            text.append('')
            text.append('Func: %s, unk1: %X, unk2: %X'%(fun['name'],fun['unk1'],fun['unk2']))
            if len(ltxt)>0:
                puretxt[Funcs[fi-1]['name']]=(ltxt[0:],lidx[0:])
                ltxt=[]
                lidx=[]
            fi+=1
            
        val=mneType[op]
        s='%08X:    '%i
        if val==0:
            text.append(s+'%s'%(mne[op]))
        elif val==1:
            text.append(s+'Call %s'%Strings[Codes[i+1]])
            i+=1
        elif val==2:
            text.append(s+'PushGlobal %s'%GlobalVars[Codes[i+1]])
            i+=1
        elif val==3:
            text.append(s+'PushStr %s'%Strings[Codes[i+2]])
            s=Strings[Codes[i+2]]
            if not isAllHalf(s):
                ltxt.append(s)
                lidx.append(i)
            i+=2
        elif val==4:
            text.append(s+'%s %X'%(mne[op],Codes[i+1]))
            i+=1
        i+=1
    if len(ltxt)>0:
        puretxt[Funcs[fi-1]['name']]=(ltxt[0:],lidx[0:])
    return (text,puretxt)
    
sc=ReadSc('scenario\\scenario.sc')
txt,puretxt=ParseSc(sc)
##fs=open('a.txt','wb')
##fs.write('\r\n'.join(txt).encode('u16'))
##fs.close()
for name in puretxt:
    ltxt,lidx=puretxt[name]
    fs=open('txt\\%s.txt'%name,'wb')
    fs.write('\r\n'.join(ltxt).encode('u16'))
    fs.close()
    fs=open('txt\\%s.idx'%name,'wb')
    for i in lidx:
        fs.write(pack('I',i))
    fs.close()
