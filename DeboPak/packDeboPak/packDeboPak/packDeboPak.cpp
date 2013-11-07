// packDeboPak.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "zlibraw.h"

#pragma warning(disable:4996)

#pragma pack(1)
typedef unsigned __int64 QWORD;

typedef struct {
	BYTE magic[4];		// "PAK\0"
	WORD index_info_offset;
	WORD unknown0;		// 6
	WORD resreved0;		// 0
	WORD zero;			// 0
	DWORD resreved1;		// 0
}PakHeader;

typedef struct{
	DWORD index_info_size;
	DWORD reserved0;		// 0
	DWORD dir_entries;
	DWORD uncomprlen;
	DWORD comprlen;
	DWORD reserved1;		// 0
}PakInfo;

typedef struct{
	QWORD offset;			// from 0
	QWORD uncomprlen;
	QWORD comprlen;
	DWORD flags;
	FILETIME CreateTime;
	FILETIME LastAccess;
	FILETIME LastWrite;
	//s8 *name;
}PakEntry;
#pragma pack()

using namespace std;

wchar_t* idxname=L"D:\\chinesize\\DeboPak\\game.idx";
wchar_t* oripakname=L"D:\\chinesize\\DeboPak\\game.pak";
wchar_t* newpath=L"D:\\chinesize\\DeboPak\\game\\";
wchar_t* newpakname=L"D:\\chinesize\\DeboPak\\game1.pak";

wchar_t** GetIdx(wchar_t* stm, DWORD size, DWORD* count)
{
	DWORD linecount=0;
	DWORD i;
	for(i=1;i<size/2;i++)
	{
		if(stm[i]==L'\r' && stm[i+1]==L'\n')
		{
			stm[i]=L'\0';
			linecount++;
		}
	}
	if(stm[size/2-1]!=L'\n')
		linecount++;
	wchar_t** lpIdxTable=new wchar_t*[linecount];

	*count=linecount;

	lpIdxTable[0]=&stm[1];
	wchar_t* p=&stm[1];
	for(i=1;i<linecount;i++)
	{
		p=wcschr(p,L'\0')+2;
		lpIdxTable[i]=p;
	}
	return lpIdxTable;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc!=5)
	{
		wprintf(L"Usage: packPak.exe <oripak> <idx> <path> <newpak>\n");
		return 0;
	}

	oripakname=argv[1];
	idxname=argv[2];
	newpath=argv[3];
	newpakname=argv[4];

	FILE* fp=_wfopen(oripakname,L"rb");
	if(fp==NULL)
	{
		wprintf(L"Can't open oripak file!");
		return 0;
	}
	PakHeader phdr;
	PakInfo pinfo;
	fread(&phdr,1,sizeof(phdr),fp);
	fseek(fp,phdr.index_info_offset,SEEK_SET);
	fread(&pinfo,1,sizeof(pinfo),fp);
	
	BYTE* compr=new BYTE[pinfo.comprlen];
	BYTE* unc=new BYTE[pinfo.uncomprlen];
	fread(compr,1,pinfo.comprlen,fp);

	DWORD basePos=phdr.index_info_offset+sizeof(pinfo)+pinfo.comprlen;
	
	int ret=uncompr_raw(unc,pinfo.uncomprlen,compr,pinfo.comprlen);
	if(ret!=Z_OK)
	{
		wprintf(L"Error occurs while decompress!");
		return 0;
	}

	FILE* fpt=_wfopen(idxname,L"rb");
	if(fpt==NULL)
	{
		wprintf(L"Can't open idx file!");
		return 0;
	}
	fseek(fpt,0,SEEK_END);
	DWORD txtsize=ftell(fpt);
	fseek(fpt,0,SEEK_SET);
	BYTE* txtsbuff=new BYTE[txtsize+1];
	fread(txtsbuff,1,txtsize,fpt);
	fclose(fpt);
	DWORD idxStrCount;
	wchar_t** lpIdxStrs=GetIdx((wchar_t*)txtsbuff,txtsize,&idxStrCount);

	PakEntry* pe=(PakEntry*)unc;
	BYTE* oldunc=new BYTE[pinfo.uncomprlen];
	memcpy(oldunc,unc,pinfo.uncomprlen);

	wstring* s=new wstring(newpath);
	if(s->back()!=L'\\')
		s->append(L"\\");
	QWORD curoff=0;
	for(DWORD i=0;i<idxStrCount;i++)
	{
		if(pe->flags&0x20)
		{
			FILE* fpItem=_wfopen((*s+lpIdxStrs[i]).c_str(),L"rb");
			if(fpItem!=NULL)
			{
				wprintf(L"Calculating %s ...\n",&lpIdxStrs[i][2]);

				fseek(fpItem,0,SEEK_END);
				DWORD ressize=ftell(fpItem);
				BYTE* resbuff=new BYTE[ressize];
				fseek(fpItem,0,SEEK_SET);
				fread(resbuff,1,ressize,fpItem);
				fclose(fpItem);

				DWORD comprsize=ressize*2;
				BYTE* comprres=new BYTE[comprsize];
				ret=compr_raw(comprres,&comprsize,resbuff,ressize);
				delete[] resbuff;
				if(ret!=Z_OK)
				{
					wprintf(L"FatalError in %s!\n",&lpIdxStrs[i][2]);
					break;
				}
				pe->offset=curoff;
				pe->uncomprlen=ressize;
				pe->comprlen=comprsize;
				curoff+=comprsize;
				delete[] comprres;
			}
			else
			{
				pe->offset=curoff;
				curoff+=pe->comprlen;
			}
		}
		pe++;
		pe=(PakEntry*)((BYTE*)pe+strlen((char*)pe)+1);
	}

	FILE* fpnew=_wfopen(newpakname,L"wb");
	if(!fpnew)
	{
		wprintf(L"Can't open newpak!");
		return 0;
	}
	fwrite(&phdr,1,sizeof(phdr),fpnew);
	BYTE* idxnewbuff=new BYTE[pinfo.uncomprlen];
	DWORD comprsize=pinfo.uncomprlen;
	ret=compr_raw(idxnewbuff,&comprsize,unc,pinfo.uncomprlen);
	if(ret!=Z_OK)
	{
		wprintf(L"Compress Index Failed!");
		return 0;
	}
	pinfo.comprlen=comprsize;
	fwrite(&pinfo,1,sizeof(pinfo),fpnew);
	fwrite(idxnewbuff,1,comprsize,fpnew);
	delete[] idxnewbuff;
	DWORD basePos2=sizeof(phdr)+sizeof(pinfo)+comprsize;

	PakEntry* peo=(PakEntry*)oldunc;
	pe=(PakEntry*)unc;
	for(DWORD i=0;i<idxStrCount;i++)
	{
		if(pe->flags&0x20)
		{
			wprintf(L"Packing %s ...\n",&lpIdxStrs[i][2]);
			FILE* fpItem=_wfopen((*s+lpIdxStrs[i]).c_str(),L"rb");
			if(fpItem!=NULL)
			{
				fseek(fpItem,0,SEEK_END);
				DWORD ressize=ftell(fpItem);
				BYTE* resbuff=new BYTE[ressize];
				fseek(fpItem,0,SEEK_SET);
				fread(resbuff,1,ressize,fpItem);
				fclose(fpItem);

				DWORD comprsize=ressize*2;
				BYTE* comprres=new BYTE[comprsize];
				ret=compr_raw(comprres,&comprsize,resbuff,ressize);
				delete[] resbuff;
				if(ret!=Z_OK)
				{
					wprintf(L"FatalError!\n");
					break;
				}
				_fseeki64(fpnew,basePos2+pe->offset,SEEK_SET);
				fwrite(comprres,1,comprsize,fpnew);
				delete[] comprres;
			}
			else
			{
				BYTE* comprres=new BYTE[(DWORD)peo->comprlen];
				_fseeki64(fp,basePos+peo->offset,SEEK_SET);
				fread(comprres,1,(DWORD)peo->comprlen,fp);
				if(pe->comprlen!=peo->comprlen)
				{
					wprintf(L"Fatal Error!");
					break;
				}
				_fseeki64(fpnew,basePos2+pe->offset,SEEK_SET);
				fwrite(comprres,1,(DWORD)peo->comprlen,fpnew);
				delete[] comprres;
			}
		}
		pe++;
		pe=(PakEntry*)((BYTE*)pe+strlen((char*)pe)+1);
		peo++;
		peo=(PakEntry*)((BYTE*)peo+strlen((char*)peo)+1);
	}
	wprintf(L"Done.\n");
	fclose(fp);
	fclose(fpnew);
	return 0;
}

