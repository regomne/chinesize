#!/usr/bin/env python3
packer_name = 'QLIE PACKv1'
default_ext = 'pack'

import os
import sys
import ctypes
import struct

def cipher_name(name):
    namelen=len(name)
    dec=namelen+0xfa
    return bytes([(name[i]^(((i+1)^dec)+(i+1)))&0xFF for i in range(namelen)])

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

def qlie_hash(stm):
    mm0=[0 for i in range(4)]
    mm2=[0 for i in range(4)]
    for i in range(int(len(stm)/8)):
        p = i * 8
        for k in range(4):
            mm2[k] = (mm2[k] & 0x0307) & 0xffff
            v1 = ((stm[p + k*2] ^ mm2[k]) & 0xff) | ((stm[p + k*2 + 1] << 8) ^ (mm2[k] & 0xff00))
            mm0[k] = (mm0[k] + v1) & 0xffff
    return (mm0[0] ^ mm0[2]) | ((mm0[1] ^ mm0[3]) << 16)

def pack(rootdir, outname):
    idx=[]
    print(outname)
    pack=open(outname, 'wb+')
    prefixlen=len(rootdir)+1
    for root, dirs, files in os.walk(rootdir):
        root=root[prefixlen:]
        if root: root+=os.path.sep
        files.sort()
        for filename in files:
            filename=root+filename
            print(filename)
            data=open(os.path.join(rootdir, filename),'rb').read()
            rawlen=len(data)
            is_compress=0
            if data[:4]==b'1PC\xff':
                print('Warning: input file is compressed raw data !')
                raise Exception()
            elif not data:
                data=b'1PC\xff\1\0\0\0\0\0\0\0\xfe\x7f\xfe\xff\0\0'
                is_compress=1
            offset=pack.tell()
            pack.write(data)
            encoded_fname = filename.replace(os.path.sep, '\\').encode('932')
            idx.append(struct.pack('H',len(encoded_fname))+cipher_name(encoded_fname)+\
                struct.pack('IIIIIII', offset&0xFFFFFFFF, offset>>32, len(data), rawlen, is_compress, 0, qlie_hash(data)))
    offset=pack.tell()
    print('writing index...')
    pack.write(b''.join(idx))
    pack.write(b'FilePackVer1.0\0\0'+struct.pack('III', len(idx), offset&0xFFFFFFFF, offset>>32))
    pack.close()
    print('%d files packed.'%len(idx))

def usage():
    print('Usage:')
    print('%s <src_dir> [out]'%exename)
    print('    out:    specify output filename, default name is src_dir+".%s"'%default_ext)
    print('            if output file is EXIST, it will be OVERWRITE!!')
    sys.exit()

print(packer_name+' Packer by ZeaS')

path, exename = os.path.split(sys.argv[0])
basename, ext = os.path.splitext(exename)

if len(sys.argv)<2:
    usage()
rootdir=os.path.realpath(sys.argv[1])
if not os.path.isdir(rootdir):
    print('Input is not directory.')
    usage()

outname=rootdir+'.'+default_ext
if len(sys.argv)>2:
    outname=sys.argv[2]
pack(rootdir, outname)
