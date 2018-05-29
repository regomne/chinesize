@echo off
rem ='''
set "pypath=D:\Program Files\Python27\python.exe"
if not exist "%pypath%" set pypath=D:\tools\python-2.6.5\python.exe
if not exist "%pypath%" ( echo I can't find "%~dp0python\python.exe" !
pause
goto :EOF
)
rem %~dp0appstarter.exe 0411 %pypath% -x "%~f0" %*
"%pypath%" -x "%~f0" %*
goto :EOF
rem '''

packer_name = 'QLIE PACKv1'
default_ext = 'pack'

import os
import sys
import ctypes
import struct

def cipher_name(name):
    namelen=len(name)
    dec=namelen+0xfa
    name=[ord(c) for c in name]
    return ''.join([chr((name[n]^(((n+1)^dec)+(n+1)))&0xFF) for n in range(namelen)])

def pack(rootdir, outname):
    idx=[]
    print outname
    pack=open(outname, 'wb+')
    prefixlen=len(rootdir)+1
    for root, dirs, files in os.walk(rootdir):
        root=root[prefixlen:]
        if root: root+='\\'
        files.sort()
        for filename in files:
            filename=root+filename
            print filename
            data=open(rootdir+'\\'+filename,'rb').read()
            rawlen=len(data)
            is_compress=0
            if data[:4]=='1PC\xff':
                print 'Warning: input file is compressed raw data !'
                raw_input()
            elif not data:
                data='1PC\xff\1\0\0\0\0\0\0\0\xfe\x7f\xfe\xff\0\0'
                is_compress=1
            offset=pack.tell()
            pack.write(data)
            idx.append(struct.pack('H',len(filename))+cipher_name(filename)+\
                struct.pack('IIIIIII', offset&0xFFFFFFFF, offset>>32, len(data), rawlen, is_compress, 0, QLIE_hash(data,len(data))))
    offset=pack.tell()
    print 'writing index...'
    pack.write(''.join(idx))
    pack.write('FilePackVer1.0\0\0'+struct.pack('III', len(idx), offset&0xFFFFFFFF, offset>>32))
    pack.close()
    print '%d files packed.'%len(idx)

def usage():
    print 'Usage:'
    print '%s <src_dir> [out]'%exename
    print '    out:    specify output filename, default name is src_dir+".%s"'%default_ext
    print '            if output file is EXIST, it will be OVERWRITE!!'
    sys.exit()

print packer_name+' Packer by ZeaS'

path, exename = os.path.split(sys.argv[0])
basename, ext = os.path.splitext(exename)

if len(sys.argv)<2:
    usage()
rootdir=os.path.realpath(sys.argv[1])
if not os.path.isdir(rootdir):
    print 'Input is not directory.'
    usage()

outname=rootdir+'.'+default_ext
if len(sys.argv)>2:
    outname=sys.argv[2]

funcbuff=[]
def create_cdecl_function(func_type, func_str):
    func_type = func_type.split(',')
    functype = eval(''.join(['ctypes.CFUNCTYPE(ctypes.c_'+func_type[0].strip()]+ # ret type
                            [', ctypes.c_%s'%arg.strip() for arg in func_type[1:]] + [')']))
    funcdata = ctypes.create_string_buffer(func_str)
    funcbuff.append(funcdata)
    return functype(ctypes.addressof(funcdata))

#DWORD gen_hash(WORD *buf, DWORD len) {
#	WORD MM0[4]={0}, MM2[4]={0};
#	unsigned int i;
#	for (i = 0; i < len / 8; i++) {
#		for (unsigned int k = 0; k < 4; k++) {
#			MM2[k] += 0x0307;
#			MM0[k] += (buf[k] ^ MM2[k]);
#		}
#		buf += 4;
#	}
#	return (MM0[0] ^ MM0[2]) | ((MM0[1] ^ MM0[3]) << 16);
#}

QLIE_hash = create_cdecl_function('uint, char_p, uint',\
	'U\x8b\xec3\xc0\x0f\xb7\xc8\x8b\xc1\xc1\xe1\x10\x0b\xc1\x83\xec\x10W\x8d}\xf8\xab'+\
	'\xab3\xc0\x0f\xb7\xc8\x8b\xc1\xc1\xe1\x10\x0b\xc1\x8d}\xf0\xab\xab\x8bE\x0c\xc1'+\
	'\xe8\3\x85\xc0t7\x8bU\x08\x8dM\xf0+\xd1\x8b\xf8V3\xc9\x8dD\r\xf0\xbe\7\3\0\0f\1'+\
	'0f\x8b0f\x8b\4\2f3\xc6f\1D\r\xf8\x83\xc1\2\x83\xf9\x08r\xdd\x83\xc2\x08Ou\xd5^\x0f'+\
	'\xb7M\xfa\x0f\xb7E\xfe\x0f\xb7U\xf83\xc1\x0f\xb7M\xfc\xc1\xe0\x103\xca\x0b\xc1_\xc9\xc3')

pack(rootdir, outname)