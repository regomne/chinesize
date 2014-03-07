def enc(buff):
    nb=bytearray(buff)
    key=b'wolegequ'
    i=1
    while i<len(buff):
        nb[i]=(nb[i]+(nb[i-1]^key[i&7]))&0xff
        i+=1
    return nb

def dec(buff):
    nb=bytearray(buff)
    key=b'wolegequ'
    i=len(buff)-1
    while i>0:
        nb[i]=(nb[i]-(nb[i-1]^key[i&7]))&0xff
        i-=1
    return nb

stm=open('cryptojs.js','rb').read()
fs=open('crypto.bin','wb')
fs.write(enc(stm))
fs.close()

fs=open('crypto2.js','wb')
fs.write(dec(enc(stm)))
fs.close()
