import bytefile

stm=bytefile.ByteIO(open('a.b','rb').read())

cnt=int(len(stm)/4)
for i in range(cnt):
    ii=stm.readu32()
    if ii>=0xe0000:
        print(ii)
