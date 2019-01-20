import zlib

fs=open('kcs_ext\\sscript.kcs','rb')
stm=fs.read()
fs.close()

ns=zlib.compress(stm)
fs=open('kcs\\sscript.kcs','ab+')
fs.seek(0x34)
fs.truncate()
fs.write(ns)
fs.close()
