import zlib

fs=open('r8.ain','rb')
stm=fs.read()
news=zlib.decompress(stm[0x10:])
fs=open('r82.ain','wb')
fs.write(news)
fs.close()
