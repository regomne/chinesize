#pragma once
#include <Windows.h>
#include <string>

int FillBmpHdr(BYTE* lp,int width,int height,int bit);

void Copy24To24(BYTE* dibDest,BYTE* dibSrc,int width,int height);
void Copy32To24(BYTE* dibDest,BYTE* dibSrc,int width,int height);
int LogWin(const wchar_t* str, ...);
int LogWin(const std::wstring& s);
HANDLE MakeFile(LPCTSTR path,BOOL isDir);
wchar_t* ToWCS(char* s,int cp);
HRESULT BasicFileOpen(void* filePath, BOOL isDir=FALSE,BOOL allowMulti=FALSE);
HRESULT BasicFileSave(void* filePath);

int EncodeBmpToPng(int width,int height,int bit,void* pallete,void* dib,MemStream* stream);
int AlphaBlend1(BYTE* dib,int width,int height);
BYTE* WINAPI SearchDecompressFunc();
