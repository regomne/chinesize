fs=open('test.ttf','rb+')
enc=open('readme.ini','rb').read()

toenc=bytearray(fs.read(len(enc)))

for i in range(len(enc)):
    toenc[i]^=enc[i]

fs.seek(0)
fs.write(toenc)
fs.close()
