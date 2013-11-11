#pragma once

#include <Windows.h>

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	__field_bcount_part(MaximumLength, Length) PWCH   Buffer;
} UNICODE_STRING, *PUNICODE_STRING;


extern HANDLE g_hProcessHeap;


typedef NTSTATUS (NTAPI *LoadDllRoutine)(PWCHAR,ULONG,PUNICODE_STRING,PHANDLE);
typedef int (__cdecl *DecoprFunc)(PVOID,LPDWORD,PVOID,ULONG,ULONG,ULONG);
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

struct MyDataStruct
{
	LoadDllRoutine loadDll;
	HANDLE dllHandle;
	UNICODE_STRING dllName;
	DecoprFunc funcAddr;
};
#define PIPE_NAME (L"\\\\.\\pipe\\fxckBGI%04X")
#define PRODUCT_VERSION L"2.1"

//#define DBG