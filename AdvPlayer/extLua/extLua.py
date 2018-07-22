import struct

def hasFull(s):
    for c in s:
        if c>=0x80:
            return True
    return False

def extLuaS(stm,start,end):
    i=start
    txt=[]
    while i<end:
        c=stm[i]
        i+=1
        if c==4 or c==0x14:
            l=stm[i]
            i+=1
            if l==0xff:
                l,=struct.unpack('<I',stm[i:i+4])
                i+=4
            s=stm[i:i+l-1]
            if hasFull(s):
                try:
                    txt.append(s.decode('utf-8').replace('\n','$n'))
                except:
                    print(hex(i))
                    return txt
            i+=l-1
        elif c==0x13:
            i+=8
        else:
            raise Exception('inst: %x, pos:%x'%(c,i))
    return txt

def packLuaS(stm,start,end,txt):
    ns=[]
    i=start
    j=0
    while i<end:
        c=stm[i]
        i+=1
        ns.append(bytes([c]))
        if c==4 or c==0x14:
            lens=stm[i]
            i+=1
            if lens==0xff:
                lens,=struct.unpack('<I',stm[i:i+4])
                i+=4
            s=stm[i:i+lens-1]
            if hasFull(s):
                l=txt[j].replace('$n','\n').encode('utf-8')
                j+=1
                if len(l)>=0xfe:
                    ns.append(b'\xff'+struct.pack('<I',len(l)+1))
                else:
                    ns.append(bytes([len(l)+1]))
                ns.append(l)
            else:
                ns.append(bytes([lens]))
                ns.append(s)
            i+=lens-1
        elif c==0x13:
            ns.append(stm[i:i+8])
            i+=8
        else:
            raise Exception('inst: %x, pos:%x'%(c,i))
    return stm[0:start]+b''.join(ns)+stm[end:]

def ext(luas,offstart,offend,outf):
    fs=open(luas,'rb')
    stm=fs.read()
    fs.close()
    txt=extLuaS(stm,offstart,offend)
    fs=open(outf,'wb')
    fs.write('\r\n'.join(txt).encode('utf-8-sig'))
    fs.close()

def pack(luas,offstart,offend,txt,outf):
    fs=open(luas,'rb')
    stm=fs.read()
    fs.close()
    txt=open(txt,'rb')
    ls=txt.read().decode('utf-8-sig').split('\r\n')
    ns=packLuaS(stm,offstart,offend,ls)
    fs=open(outf,'wb')
    fs.write(ns)
    fs.close()

##ext(r'd:\Game\見上げてごらん、夜空の星を Interstellar Focus\Script\LegacyGame_utf8.lua',
##    0xbba,
##    0x47a2,
##    'a.txt')

pack(r'd:\Game\見上げてごらん、夜空の星を Interstellar Focus\Script\LegacyGame_utf8.lua',
     0xbba,
     0x47a2,
     'a.txt',
     'LegacyGame_utf8.lua')
