#pragma once
#include <windows.h>

struct MemStream
{
    BYTE* start;
    BYTE* cur;
    DWORD len;
};

void InitCrcTable();
int ReadPngToBmp(MemStream& src,int* width,int* height,int* bit,BYTE** dib);

int __cdecl NewExtR6ti(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom);
int __cdecl NewExtR7(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,
                     DWORD lefttop2,DWORD rightbottom2,BOOL unk);
int __cdecl NewExtRip008(BYTE* src,int srcLen,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,
                     DWORD lefttop2,DWORD rightbottom2,BOOL unk);


typedef int (__cdecl *ExPic7Routine)(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,DWORD,DWORD,BOOL);
typedef int (__cdecl *ExPicR8Routine)(BYTE* src,int srcLen,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,DWORD,DWORD,BOOL);
typedef int (__cdecl *ExPic6Routine)(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom);

extern ExPic6Routine oldextR6ti;
extern ExPic7Routine oldExtPicType7;
extern ExPicR8Routine oldExtPicTypeRip008;
