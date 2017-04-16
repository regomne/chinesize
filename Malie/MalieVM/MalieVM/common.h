#pragma once
#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// #pragma comment(linker, "/SECTION:.text,ERW /MERGE:.rdata=.text /MERGE:.data=.text")
// #pragma comment(linker, "/SECTION:.Azure,ERW /MERGE:.text=.Azure")

#ifdef kDays
#define DbgPrint(x) OutputDebugStringA(x);
#else
#define DbgPrint(x) ;
#endif // kDays

basic_string<CHAR> stringf(CHAR* format,...);
basic_string<WCHAR> stringf(WCHAR* format,...);

unsigned long crc32(char str[],int len);


void ZwContinueHook();

void * operator new (size_t size);
void * operator new [](size_t size);
void operator delete (void * pointer);
void operator delete[](void * pointer);