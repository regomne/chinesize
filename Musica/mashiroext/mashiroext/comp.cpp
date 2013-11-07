#include"mashiroext.h"

#include"blowfish2.h"
#include <stdio.h>
#include<algorithm>
#include<list>
using namespace std;

struct PAZENTRY2
{
	string fn;
	struct PAZENTRY ent;
};

extern "C" const game_info_t GAME_INFO;

int Compact(TCHAR* szFolder,TCHAR* szFileName)
{
	DWORD temp;
	if(!::SetCurrentDirectory(szFolder))
		return -1;

	char* p=szFolder;
	p+=strlen(szFolder);
	if(*(p-1)=='\\')
		p--;
	while(*--p!='\\');
	string prefix=++p;

	p=szFolder+strlen(szFolder);
	if(*(p-1)=='\\')
		--p;
	strcpy(p,".dat");

	key_info_t  keys;
	for (unsigned long i = 0; !GAME_INFO.keys[i].prefix.empty(); i++)
	{
		if (prefix == GAME_INFO.keys[i].prefix)
		{
			keys = GAME_INFO.keys[i];
		}
	}
	if (keys.prefix.empty())
		return -1;

	bool is_audio = prefix == "bgm" || prefix == "se" || prefix == "voice" || prefix == "PMvoice";
	bool is_mov   = prefix == "mov";
	if(is_mov)
		return -1;

	PAZHDR hdr;
//	strcpy((char*)&hdr,"mashiroiro");

	HANDLE hOldFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hOldFile==INVALID_HANDLE_VALUE)
		return -1;
	ReadFile(hOldFile,&hdr,sizeof(hdr),&temp,0);
	char simple_key = hdr.toc_len >> 24;
	if(simple_key)
		return -1;
	char* toc_buff2=new char[hdr.toc_len];
	ReadFile(hOldFile,toc_buff2,hdr.toc_len,&temp,0);
	CloseHandle(hOldFile);

	CBlowFish bf;
	bf.Initialize(keys.toc_key,sizeof(keys.toc_key));
	bf.Decode((unsigned char*)toc_buff2,(unsigned char*)toc_buff2,hdr.toc_len);
	list<PAZENTRY2> entries;
	list<PAZENTRY2>::iterator ent_itr,ent_itr_end;
	char*p2=toc_buff2;
	int nEntryCount=*(long*)p2;
	p2+=4;
	for(int i=0;i<nEntryCount;i++)
	{
		PAZENTRY2 entry;
		entry.fn=p2;
		p2+=entry.fn.length()+1;
		memcpy(&entry.ent,p2,sizeof(entry.ent));
		p2+=sizeof(entry.ent);
		entries.push_back(entry);
	}
	delete[] toc_buff2;

	//int nSizeEntries=sizeof(PAZHDR2);
	//WIN32_FIND_DATA stFindData;

	//HANDLE hFind=FindFirstFile(TEXT("*"),&stFindData);
	//if(hFind!=INVALID_HANDLE_VALUE)
	//	do
	//	{
	//		PAZENTRY2 entry;
	//		HANDLE hNowFile=CreateFile(stFindData.cFileName,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	//		if(hNowFile==INVALID_HANDLE_VALUE)
	//			continue;
	//		entry.fn=stFindData.cFileName;
	//		entry.ent.length =GetFileSize(hNowFile,NULL);
	//		entry.ent.original_length=entry.ent.length ;
	//		entry.ent.padded_length   = (entry.ent.length + 7) & ~7;

	//		entries.push_back(entry);

	//		nSizeEntries+=entry.fn.length ()+1+sizeof(PAZENTRY);
	//		CloseHandle(hNowFile);
	//	}while(FindNextFile(hFind,&stFindData));
	//else
	//	return -1;
	//FindClose(hFind);

//	nSizeEntries=(nSizeEntries + 7) & ~7;
	int nNowOffset=sizeof(PAZHDR)+hdr.toc_len;
	ent_itr=entries.begin();
	ent_itr_end=entries.end();
	char* toc_buff=new char[hdr.toc_len];
	memset(toc_buff,0,hdr.toc_len);
	*(long*)toc_buff=entries.size();
	for(char* p=toc_buff+sizeof(PAZHDR2);ent_itr!=ent_itr_end;ent_itr++)
	{
		ent_itr->ent.offset=nNowOffset;
		nNowOffset+=ent_itr->ent.padded_length;
		strcpy(p,ent_itr->fn.c_str());
		p+=ent_itr->fn.length()+1;
		memcpy(p,&ent_itr->ent,sizeof(ent_itr->ent));
		p+=sizeof(ent_itr->ent);
	}

	HANDLE hNewFile=CreateFile(szFolder,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hNewFile==INVALID_HANDLE_VALUE)
		return -1;

//	hdr.toc_len=nSizeEntries;
	WriteFile(hNewFile,&hdr,sizeof(hdr),&temp,0);

	bf.Initialize(keys.toc_key,sizeof(keys.toc_key));
	bf.Encode((unsigned char*)toc_buff,(unsigned char*)toc_buff,hdr.toc_len);
	WriteFile(hNewFile,toc_buff,hdr.toc_len,&temp,0);

	delete[] toc_buff;

	ent_itr=entries.begin();
	ent_itr_end=entries.end();
	for(;ent_itr!=ent_itr_end;ent_itr++)
	{
		string filename=ent_itr->fn;
		HANDLE hNowFile=CreateFile(filename.c_str(),GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if(hNowFile==INVALID_HANDLE_VALUE)
			return -1;

		char* buff=new char[ent_itr->ent.padded_length];
		memset(buff,0,ent_itr->ent.padded_length);
		ReadFile(hNowFile,buff,ent_itr->ent.length,&temp,0);
		CloseHandle(hNowFile);

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
		wsprintf(entrylen," %08X ",ent_itr->ent.length);
		transform(filename.begin (),filename.end (),filename.begin (),tolower);
		string seed=filename+(string)entrylen+crud;

		if(is_mov)
			return -1;
		else
		{
			bf.Initialize(keys.dat_key,sizeof(keys.dat_key));
			if(!crud.empty())
				unobfuscate3(seed,(unsigned char*)buff,ent_itr->ent.padded_length);
			bf.Encode((unsigned char*)buff,(unsigned char*)buff,ent_itr->ent.padded_length);
		}
		WriteFile(hNewFile,buff,ent_itr->ent.padded_length,&temp,0);
		delete[] buff;
	}
	SetEndOfFile(hNewFile);
	CloseHandle(hNewFile);
	return 0;
}