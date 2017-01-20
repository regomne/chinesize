# -*- coding: utf-8 -*-
import MjoParser
import bytefile

fname='start'
fs=open(fname+'.mjo','rb')
stm=bytefile.ByteFile(fs.read())
mp=MjoParser.MjoParser(stm)
txt=mp.Parse()
fs=open(fname+'.txt','wb')
fs.write(txt.encode('u16'))
fs.close()
fs=open(fname+'.dec','wb')
fs.write(mp.vmcode[0:])
fs.close()
