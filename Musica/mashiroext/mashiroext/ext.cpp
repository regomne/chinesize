#include"mashiroext.h"

#include"blowfish2.h"
#include<modes.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "arc4.h"
using namespace CryptoPP;

extern "C" const game_info_t GAME_INFO=
	{ "ましろ色シンフォニー",
    { { "bg",      { 0x26, 0x0C, 0x3A, 0x4A, 0x0A, 0xCA, 0x20, 0xE3, 0x94, 0xFF, 0xF6, 0x2C, 0xF3, 0x43, 0xA4, 0x3C, 0x0A, 0xE2, 0xD9, 0x03, 0x62, 0xC6, 0xE4, 0xB8, 0xBC, 0x78, 0x04, 0xBC, 0xEA, 0xC2, 0x72, 0x9F, },
                   { 0x80, 0xD2, 0x43, 0x0B, 0xEB, 0xBF, 0x41, 0x1C, 0x27, 0x77, 0xF1, 0xF5, 0xE0, 0x4D, 0x86, 0x32, 0xBC, 0xD8, 0x88, 0x56, 0xD2, 0x65, 0x65, 0x7D, 0xB0, 0xC5, 0xB7, 0xA8, 0x04, 0xD9, 0x66, 0xC5, } },
      { "bgm",     { 0xA7, 0x46, 0x3B, 0x2A, 0x85, 0x57, 0xC3, 0x82, 0xA8, 0x4F, 0xDC, 0xBD, 0x50, 0x63, 0xC1, 0x72, 0x6A, 0x0D, 0xFF, 0x7E, 0x79, 0x35, 0x69, 0x5D, 0xA8, 0xE4, 0x23, 0x3B, 0x8F, 0x56, 0xA2, 0xEA, },
                   { 0x42, 0x38, 0x0d, 0x51, 0x11, 0xe7, 0x82, 0x08, 0xe1, 0x47, 0x8c, 0x0a, 0x3f, 0x8f, 0xc5, 0x69, 0x4a, 0xfe, 0x21, 0x80, 0x43, 0xf3, 0xa2, 0x9e, 0x13, 0xd0, 0x84, 0xd5, 0x6b, 0x53, 0x82, 0x8e, } },
      { "mov",     { 0x7E, 0xE1, 0x70, 0x06, 0x09, 0x98, 0x36, 0x50, 0xD0, 0xAE, 0xD4, 0x87, 0x1E, 0xC9, 0x23, 0xEC, 0xA4, 0x72, 0x0B, 0x63, 0xCF, 0x2E, 0xD0, 0x10, 0xE3, 0xF6, 0x7C, 0x56, 0x5A, 0xD9, 0x48, 0x2F, },
                   { 0 } },
      { "scr",     { 0xF4, 0x23, 0xA8, 0xC4, 0xA3, 0x22, 0xF6, 0xFD, 0xF9, 0x76, 0xC3, 0x46, 0x74, 0xB6, 0x34, 0xE7, 0x75, 0x80, 0x2D, 0x38, 0x2C, 0x06, 0xAA, 0x10, 0x85, 0xDC, 0xCF, 0x0C, 0xE9, 0x87, 0xE6, 0xEE, },
                   { 0x85, 0xA5, 0x2B, 0xBA, 0xCF, 0x89, 0x3D, 0xA5, 0xB1, 0xDA, 0x1A, 0x24, 0xA1, 0xD4, 0x58, 0x24, 0x3F, 0x48, 0x54, 0xED, 0xEC, 0xB2, 0xC4, 0xAF, 0xDE, 0xCD, 0xD9, 0x66, 0xB7, 0x51, 0xB4, 0x2A, } },
      { "se",      { 0x62, 0x45, 0xDF, 0xE9, 0x78, 0x0F, 0xAE, 0xDA, 0x20, 0xCA, 0xE8, 0xC2, 0xF2, 0x1B, 0xDF, 0x1A, 0x50, 0xDB, 0x0F, 0xE4, 0x2C, 0xEC, 0x1C, 0xCB, 0x10, 0x0A, 0x83, 0x7B, 0xE9, 0xE5, 0x19, 0x79, },
                   { 0x43, 0x84, 0x86, 0xc5, 0x65, 0x53, 0x0b, 0xb2, 0x50, 0xee, 0xc8, 0xa6, 0x4c, 0xcd, 0xc9, 0x17, 0xe5, 0x05, 0x2d, 0xa3, 0xae, 0xbd, 0xc9, 0xdc, 0x0c, 0x69, 0x1b, 0x04, 0xe9, 0x93, 0x61, 0x5c, } },
      { "st",      { 0x65, 0x20, 0x30, 0xA3, 0x50, 0xE7, 0xAE, 0x1F, 0x5D, 0xDD, 0x8A, 0x89, 0x8B, 0xEC, 0xD4, 0x90, 0x8E, 0xFD, 0x22, 0x76, 0x6B, 0x48, 0x3B, 0x8E, 0x81, 0x4E, 0x48, 0x97, 0x7D, 0xB1, 0xF9, 0x0B, },
                   { 0x0D, 0xAA, 0x7A, 0x9E, 0xCD, 0xA1, 0xE6, 0x02, 0x92, 0x12, 0xE1, 0xD1, 0x9C, 0xD6, 0x3C, 0xCE, 0xB3, 0x5F, 0x96, 0x13, 0xF7, 0x9D, 0xF6, 0xD1, 0x3D, 0x60, 0x79, 0xC3, 0x88, 0x21, 0x0B, 0xF0, } },
      { "sys",     { 0xAC, 0x44, 0x67, 0x8A, 0x3C, 0x93, 0xF6, 0xC9, 0x79, 0xDF, 0x29, 0xEC, 0x84, 0x52, 0x4A, 0x2C, 0xC2, 0x91, 0x9F, 0x89, 0xC0, 0xEB, 0xD5, 0x7F, 0x2C, 0x60, 0xF1, 0x02, 0xC0, 0x59, 0x65, 0xE1, },
                   { 0x7A, 0x1D, 0x0B, 0xBE, 0xA0, 0x17, 0x4E, 0x5E, 0xD3, 0xDF, 0xAD, 0x9D, 0x05, 0xAB, 0x1B, 0xAB, 0x68, 0x7C, 0x7A, 0x1A, 0x07, 0x9A, 0xE6, 0x5F, 0x98, 0xBF, 0xC7, 0x3B, 0xD3, 0x04, 0xF1, 0xBB, } },
      { "voice",   { 0xE8, 0xF1, 0x93, 0x09, 0x94, 0x47, 0x00, 0x8C, 0x77, 0xB4, 0x3B, 0xBA, 0x74, 0xE4, 0xF5, 0x63, 0xB7, 0x39, 0x2E, 0x67, 0x21, 0xD6, 0x8D, 0x16, 0x28, 0xE0, 0x51, 0x99, 0x6F, 0x97, 0x80, 0x98, },
                   { 0x40, 0xa0, 0x21, 0x09, 0xb3, 0xb4, 0xf7, 0x4d, 0xc4, 0x1a, 0xf9, 0xad, 0xc3, 0x03, 0x28, 0xd2, 0xcd, 0x61, 0xb7, 0x9a, 0xe2, 0x65, 0xf5, 0x0a, 0x4e, 0x9c, 0xe8, 0xc2, 0x3c, 0xc8, 0x96, 0x25, } }, },
    { "hFx1Bh93", "cQ5jhgpb", "rytKRa38", "ChFdc6iA" }, };

void unobfuscate3(const string& filename, unsigned char* buff, unsigned long len) {
  char scramble[256] = { 0 };

  for (unsigned long i = 0; i < 256; i++) {
    scramble[i] = (unsigned char) i;
  }

  size_t        filename_len = filename.length();
  unsigned long index   = 0;

  for (unsigned long i = 0; i < 256; i++) {
    index = (unsigned char) (index + scramble[i] + filename[i % filename_len]);

    std::swap(scramble[i], scramble[index]);
  }

  unsigned long index1 = 0;
  unsigned long index2 = 0;

  for (unsigned long i = 0; i < len; i++) {
    index1 = (unsigned char) (index1 + 1);
    index2 = (unsigned char) (index2 + scramble[index1]);
    std::swap(scramble[index1], scramble[index2]);

    buff[i] ^= scramble[(unsigned char)(scramble[index1] + scramble[index2])];
  }
}

void write_obfuscate(char* curloc, unsigned char key, void* buff, unsigned long len) {
  unsigned char* out_buff = new unsigned char[len];

  unsigned char* out_p   = (unsigned char*) out_buff;
  unsigned char* out_end = out_p + len;
  unsigned char* p       = (unsigned char*) buff;

  while (out_p < out_end) {
    *out_p++ = *p++ ^ key;
  }
  
  //write(fd, out_buff, len);
  memcpy(curloc,out_buff,len);

  delete [] out_buff;
}

void read_unobfuscate(char* curloc, unsigned char key, void* buff, unsigned long len) {
  //read(fd, buff, len);
	memcpy(buff,curloc,len);

  unsigned char* p   = (unsigned char*) buff;
  unsigned char* end = p + len;

  while (p < end) {
    *p++ ^= key;
  }
}

void decrypt_mov(unsigned char* seed,
	const string&  seed2,
	unsigned char* buff,
	unsigned long  len)
{
	unsigned char rc4_key[256];

	for (unsigned long i = 0; i < 256; i++) {
		rc4_key[i] = seed[i] ^ seed2[i % seed2.length()];
	}

	unsigned long  key_len = 0x10000;
	unsigned char* key     = new unsigned char[key_len];
	CryptoPP::Weak::ARC4 rc4(rc4_key, sizeof(rc4_key));     
	rc4.GenerateBlock(key, key_len);

	for (unsigned long i = 0; i < len; i++) {
		buff[i] ^= key[i % key_len];
	}

	delete [] key;
}


int Extract(TCHAR* szFileName)
{
	int temp;
	HANDLE hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile==INVALID_HANDLE_VALUE)
		return -1;
	LARGE_INTEGER liFileSize;
	GetFileSizeEx(hFile,&liFileSize);
	HANDLE hMap=CreateFileMapping(hFile,0,PAGE_READONLY,liFileSize.HighPart,liFileSize.LowPart,NULL);
	if(!hMap)
	{
		CloseHandle(hFile);
		return -1;
	}
	char* lpOri=(char*)MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
	if(!lpOri)
	{
		CloseHandle(hFile);
		CloseHandle(hMap);
		return -1;
	}
	
	char* p2=new char[20];
	char* p3=p2;
	{
		char*pend=szFileName+strlen(szFileName);
		char *p=pend;
		while(*p--!='\\');
		p+=2;
		while(*p++!='.'&&p<pend)
			*p3++=*(p-1);
		*p3='\0';
	}
	string prefix(p2);
	delete[] p2;
	key_info_t keys;

	for (unsigned long i = 0; !GAME_INFO.keys[i].prefix.empty(); i++) 
	{
		if (prefix == GAME_INFO.keys[i].prefix) 
		{
			keys = GAME_INFO.keys[i];
		}
	}
	if (keys.prefix.empty()) 
	{
		return -1;
	}
	bool is_audio = prefix == "bgm" || prefix == "se" || prefix == "voice" || prefix == "PMvoice";
	bool is_mov   = prefix == "mov";
	  
	char* pcuroff=lpOri;

	PAZHDR hdr;
	//read(fd, &hdr, sizeof(hdr));  
	memcpy(&hdr,pcuroff,sizeof(hdr));

	char simple_key = hdr.toc_len >> 24;

	read_unobfuscate(pcuroff, simple_key, &hdr, sizeof(hdr));
	pcuroff+=sizeof(hdr);

	unsigned char* toc_buff = new unsigned char[hdr.toc_len];
//	unsigned char* toc_buff2 = new unsigned char[hdr.toc_len];
	read_unobfuscate(pcuroff, simple_key, toc_buff, hdr.toc_len);
	pcuroff+=hdr.toc_len;

	CBlowFish bf;
	bf.Initialize (keys.toc_key, sizeof(keys.toc_key));
	bf.Decode(toc_buff,toc_buff,hdr.toc_len);

	PAZHDR2*       hdr2 = (PAZHDR2*) toc_buff;
	unsigned char* p    = (unsigned char*) (hdr2 + 1);

	//DWORD tmpp;
	//HANDLE tmp=CreateFile("a.dat",GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	//WriteFile(tmp,toc_buff,hdr.toc_len,&tmpp,0);
	//CloseHandle(tmp);


	unsigned char* mov_seed = NULL;

	if (is_mov)
	{
		mov_seed = p;
		p += 256;
	}

	for (unsigned long i = 0; i < hdr2->entry_count; i++)
	{
		string filename = (char*) p;
		p += filename.length() + 1;

		PAZENTRY* entry = (PAZENTRY*) p;
		p += sizeof(*entry); 

		unsigned long  len  = 0;
		unsigned char* buff = NULL;

		len  = entry->padded_length;
		buff = new unsigned char[len];
		//lseek(fd, entry->offset, SEEK_SET);
		pcuroff=lpOri+entry->offset;
		read_unobfuscate(pcuroff, simple_key, buff, len);

		string crud;

		if (filename.find(".png") != string::npos)
		{
			crud = GAME_INFO.crud.png;
		}
		else if (filename.find(".ogg") != string::npos || is_audio)
		{
			crud = GAME_INFO.crud.ogg;
		}
		else if (filename.find(".sc") != string::npos) 
		{
			crud = GAME_INFO.crud.sc;
		}
		else if (filename.find(".avi") != string::npos || filename.find(".mpg") != string::npos)
		{
			crud = GAME_INFO.crud.avi;
		}

		char entrylen[12];
		wsprintf(entrylen," %08X ",entry->length);
		transform(filename.begin (),filename.end (),filename.begin (),tolower);
		string seed=filename+(string)entrylen+crud;

		if (is_mov)
		{
			decrypt_mov(mov_seed, seed, buff, len);
		}
		else 
		{
			//bfD.SetKey (keys.dat_key ,sizeof(keys.dat_key ));
			//bfD.ProcessData (buff,buff, len);
			bf.Initialize(keys.dat_key ,sizeof(keys.dat_key ));
			bf.Decode(buff,buff,len);
			if (!crud.empty()) 
				unobfuscate3(seed, buff, len);
		}

		char* path=new char[strlen(szFileName)+1];
		char* ppath=path+strlen(szFileName);
		strcpy(path,szFileName);
		while(*--ppath!='.');
		*ppath='\0';

		if(!::SetCurrentDirectory(path))
		{
			CreateDirectory(path,0);
			SetCurrentDirectory(path);
		}
		delete[] path;
		wchar_t *fn=new wchar_t[filename.length()+1];
		MultiByteToWideChar(932,0,filename.c_str(),-1,fn,filename.length()+1);
		HANDLE hNewFile=_MakeFile(fn);
		//HANDLE hNewFile=CreateFile(filename.c_str(),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		
		if(hNewFile==INVALID_HANDLE_VALUE)
			continue;
		WriteFile(hNewFile,buff,entry->length ,(LPDWORD)&temp,0);
		CloseHandle(hNewFile);
		delete[] buff;
	}
	return 0;
}
