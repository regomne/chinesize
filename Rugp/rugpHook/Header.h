#pragma once

#include <Windows.h>

struct StringStrt
{
	DWORD offset;
	DWORD hash;
	DWORD hash2;
	BYTE* str;
};

struct HashFilePair
{
	DWORD hash;
	union
	{
		DWORD fileNum;
		StringStrt* sTbl;
	};
	DWORD count;
};

struct HashStrEntry
{
    DWORD hash;
    union
    {
        DWORD idx;
        BYTE* str;
    };

};

struct PatchStruct
{
	DWORD address;
	void* buff;
	int len;
};

extern HANDLE hGlobalHeap;

BOOL InitApiHooks();
extern WORD MapTbl[65536];

void *bsearch( const void *key, const void *base, size_t num, size_t width, int ( __cdecl *compare ) ( const void *, const void *) );
typedef DWORD (__cdecl *ReadMsgStringFunc)(BYTE*,CHAR*);
void Log(WCHAR*);
int __cdecl compare1(const void * v1, const void * v2);
int PatchMemory(PatchStruct* strt);

BYTE* ReadFileToMem(const WCHAR* fName,DWORD* pSize);
DWORD __stdcall CalcHash(void* buf,DWORD len);

#define MUV_EXTRA
//#define TEST

#ifdef MUV_EXTRA
#define FILE_COUNT 106
#define NAMES_COUNT 15
#else
#define FILE_COUNT 77
#define NAMES_COUNT 21
#endif