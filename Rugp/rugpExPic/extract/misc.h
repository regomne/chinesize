#pragma once
#include <Windows.h>
#include <string>

struct MemStream
{
    BYTE* start;
    BYTE* cur;
    DWORD len;
};

int FillBmpHdr(BYTE* lp,int width,int height,int bit);

void Copy24To24(BYTE* dibDest,BYTE* dibSrc,int width,int height);
HANDLE MakeFile(LPCTSTR path,BOOL isDir);
wchar_t* ToWCS(char* s,int cp);

int EncodeBmpToPng(int width,int height,int bit,void* pallete,void* dib,MemStream* stream);
int AlphaBlend1(BYTE* dib,int width,int height);
