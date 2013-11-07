#include"mashiroext.h"

#include"blowfish2.h"
#include <Windows.h>
#include <stdio.h>
#include<algorithm>
#include<list>
#include<vector>
#include<string>
using namespace std;

extern "C" const game_info_t GAME_INFO;
extern "C" const BYTE MovBlock[256];
extern HWND g_hWinMain;

char errmes[100];

struct PAZENTRY2
{
	string fn;
	struct PAZENTRY ent;
};

typedef __int64 QWORD;
struct MyPazEnt
{
	char* fn;
	QWORD offset;
	DWORD uncomprlen;
	DWORD comprlen;
	DWORD decryptlen;
	WORD iscompr;
	WORD unk;
};


void GetDirectoryFiles(LPTSTR lpszPath,vector<LPTSTR>* pFiles,int npos)
{
	int len=lstrlen(lpszPath);
	if(lpszPath[len-1]=='\\')
		lpszPath[len-1]='\0';
	TCHAR* lpszRPath=new TCHAR[len+5];
	lstrcpy(lpszRPath,lpszPath);
	lstrcat(lpszRPath,TEXT("\\*"));
	WIN32_FIND_DATA* pfd=new WIN32_FIND_DATA;
	HANDLE hFind=FindFirstFile(lpszRPath,pfd);
	delete[] lpszRPath;
	if(hFind!=INVALID_HANDLE_VALUE)
	{
		do
		{
			if(pfd->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if(!lstrcmp(pfd->cFileName,TEXT(".")) ||
					!lstrcmp(pfd->cFileName,TEXT("..")))
					continue;
				TCHAR* lpszNewPath=new TCHAR[lstrlen(lpszPath)+lstrlen(pfd->cFileName)+2];
				lstrcpy(lpszNewPath,lpszPath);
				lstrcat(lpszNewPath,TEXT("\\"));
				lstrcat(lpszNewPath,pfd->cFileName);
				GetDirectoryFiles(lpszNewPath,pFiles,npos);
				delete[] lpszNewPath;
			}
			else
			{
				TCHAR* lpszFn=new TCHAR[len-npos+lstrlen(pfd->cFileName)+2];
				if(npos>=lstrlen(lpszPath))
					lstrcpy(lpszFn,TEXT(""));
				else
				{
					lstrcpy(lpszFn,&lpszPath[npos]);
					lstrcat(lpszFn,TEXT("\\"));
				}
				lstrcat(lpszFn,pfd->cFileName);
				pFiles->push_back(lpszFn);
			}
		} while (FindNextFile(hFind,pfd));
		FindClose(hFind);
	}
	delete pfd;
}

void FreeDirectoryFiles(vector<LPTSTR>* pf)
{
	for (int i=0;i<pf->size();i++)
	{
		delete[] (*pf)[i];
	}
}

void Mbe(LPTSTR mes)
{
	MessageBox(g_hWinMain,mes,0,0);
}

string* GetPrefix(char* lpszFdr)
{
	char* p=lpszFdr+strlen(lpszFdr);
	while(*--p!='\\')
		if(p<=lpszFdr)
			break;

	char* pend=lpszFdr+strlen(lpszFdr);
	char* p2=++p;
	while(p<=pend)
	{
		if(*p>='0' && *p<='9')
		{
			pend=p;
			break;
		}
		p++;
	}
	string* s=new string(p2,pend-p2);
	return s;
}

string* MakeSeed(MyPazEnt* pe)
{
	string crud;
	string filename=pe->fn;

	if (filename.find(".png") != string::npos)
	{
		crud = GAME_INFO.crud.png;
	}
	else if (filename.find(".ogg") != string::npos)
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
	else
	{
		return NULL;
	}

	char entrylen[12];
	wsprintf(entrylen," %08X ",pe->uncomprlen);
	transform(filename.begin (),filename.end (),filename.begin (),tolower);
	string* seed=new string(filename+(string)entrylen+crud);
	return seed;
}

int Compact(TCHAR* szFolder,TCHAR* szFileName)
{
	DWORD temp;
	int len=lstrlen(szFolder);
	if(szFolder[len-1]=='\\')
		szFolder[len-1]='\0';

	if(!(GetFileAttributes(szFolder)&FILE_ATTRIBUTE_DIRECTORY))
	{
		Mbe(TEXT("目录错误"));
		return -1;
	}

	vector<LPTSTR> vFiles;
	GetDirectoryFiles(szFolder,&vFiles,lstrlen(szFolder)+1);

	string* prefix=GetPrefix((char*)szFolder);

	key_info_t  keys;
	for (unsigned long i = 0; !GAME_INFO.keys[i].prefix.empty(); i++)
	{
		if (*prefix == GAME_INFO.keys[i].prefix)
		{
			keys = GAME_INFO.keys[i];
		}
	}
	if (keys.prefix.empty())
	{
		Mbe(TEXT("请使用scr,mov等游戏目录中已存在的目录名"));
		delete prefix;
		FreeDirectoryFiles(&vFiles);
		return -1;
	}

	bool is_audio = *prefix == "bgm" || *prefix == "se" || *prefix == "voice" || *prefix == "PMvoice";
	bool is_mov   = *prefix == "mov";

	if(!is_mov)
	{
		Mbe(TEXT("只能封装mov"));
		delete prefix;
		FreeDirectoryFiles(&vFiles);
		return -1;
	}

	delete prefix;

	int idxlen=4;
	for(int i=0;i<vFiles.size();i++)
	{
		idxlen+=lstrlen(vFiles[i])+1+0x18;
	}
	if(is_mov)
		idxlen+=256;

	HANDLE hNewFile=CreateFile(szFileName,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hNewFile==INVALID_HANDLE_VALUE)
	{
		Mbe(TEXT("无法访问封包文件！"));
		FreeDirectoryFiles(&vFiles);
		return -1;
	}

	MyPazEnt* ents=new MyPazEnt[vFiles.size()];
	MyPazEnt* pe=ents;
	DWORD offset=((idxlen+7)&~7)+sizeof(PAZHDR);
	CBlowFish bf;
	string rootpath=szFolder;
	rootpath+="\\";
	for(int i=0;i<vFiles.size();i++)
	{
		pe->fn=vFiles[i];
		HANDLE hf=CreateFile((rootpath+vFiles[i]).c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if(hf==INVALID_HANDLE_VALUE)
		{
			delete[] ents;
			FreeDirectoryFiles(&vFiles);
			sprintf(errmes,"无法访问%s！",vFiles[i]);
			Mbe(errmes);
			return -1;
		}

		DWORD fsize=GetFileSize(hf,0);
		pe->comprlen=fsize;
		pe->uncomprlen=fsize;
		pe->decryptlen=(fsize+7)&~7;
		pe->unk=0;
		pe->iscompr=0;
		pe->offset=offset;
		offset+=pe->decryptlen;

		BYTE* buf=new BYTE[pe->decryptlen];
		memset(buf+pe->decryptlen-8,0,8);
		ReadFile(hf,buf,fsize,&temp,0);

		string* seed=MakeSeed(pe);

		if(is_mov)
		{
			decrypt_mov(MovBlock,*seed,buf,pe->decryptlen);
			delete seed;
		}
		else
		{
			if(seed!=NULL)
			{
				unobfuscate3(*seed,buf,pe->decryptlen);
				delete seed;
			}

			bf.Initialize(keys.dat_key,sizeof(keys.dat_key));
			bf.Encode(buf,buf,pe->decryptlen);
		}

		SetFilePointer(hNewFile,pe->offset,0,FILE_BEGIN);
		WriteFile(hNewFile,buf,pe->decryptlen,&temp,0);
		
		delete[] buf;
		CloseHandle(hf);
		pe++;
	}

	PAZHDR hdr;
	hdr.toc_len=(idxlen+7)&~7;
	SetFilePointer(hNewFile,0,0,FILE_BEGIN);
	WriteFile(hNewFile,&hdr,sizeof(hdr),&temp,0);

	BYTE* idxbuff=new BYTE[idxlen];
	BYTE* p=idxbuff;
	*(DWORD*)p=vFiles.size();
	p+=4;
	if(is_mov)
	{
		memcpy(p,MovBlock,256);
		p+=256;
	}
	pe=ents;
	for(int i=0;i<vFiles.size();i++)
	{
		strcpy((char*)p,pe->fn);
		p+=strlen(pe->fn)+1;
		memcpy(p,&(pe->offset),0x18);
		p+=0x18;
		pe++;
	}
	bf.Initialize(keys.toc_key,sizeof(keys.toc_key));
	bf.Encode(idxbuff,idxbuff,hdr.toc_len);

	WriteFile(hNewFile,idxbuff,idxlen,&temp,0);

	CloseHandle(hNewFile);
	delete[] idxbuff;
	delete[] ents;
	FreeDirectoryFiles(&vFiles);
	return 0;
}