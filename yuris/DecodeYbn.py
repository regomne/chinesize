import os
import struct
import bytefile

key=[0xd3, 0x6f, 0xac, 0x96]

def decodeYbn(ybn):
	if ybn.read(4)!=b'YSTB':
		return ybn[0:]
	ybn.seek(12)
	code_size, arg_size, res_size, offlist_size=struct.unpack('IIII4x',ybn.read(20))
	decode=[ybn[0:0x20]]
	for size in (code_size,arg_size,res_size,offlist_size):
		#print '%x'%size
		for n in range(size):
			decode.append(bytes([ybn.readu8()^key[n&3]]))
	return b''.join(decode)

path1='ys1'
path2='ys1d'
if not os.path.exists(path2):
        os.makedirs(path2)
for f in os.listdir(path1):
        fs=open(os.path.join(path1,f),'rb')
        stm=bytefile.ByteIO(fs.read())
        fs.close()
        fs=open(os.path.join(path2,f),'wb')
        fs.write(decodeYbn(stm))
        fs.close()
