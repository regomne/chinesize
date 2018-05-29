#pragma once

#include <Windows.h>

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	__field_bcount_part(MaximumLength, Length) PWCH   Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

typedef NTSTATUS (NTAPI *LoadDllRoutine)(PWCHAR,ULONG,PUNICODE_STRING,PHANDLE);
typedef int (__cdecl *DecoprFunc)(PVOID,LPDWORD,PVOID,ULONG,ULONG,ULONG);
int InjectToProcess(HANDLE process,HANDLE thread,wchar_t* dllPath,DecoprFunc func);


struct MyDataStruct
{
	LoadDllRoutine loadDll;
	HANDLE dllHandle;
	UNICODE_STRING dllName;
};

extern "C"
{
	NTSYSAPI NTSTATUS NTAPI NtGetContextThread(
		HANDLE              ThreadHandle,
		PCONTEXT            pContext );

	NTSYSAPI NTSTATUS NTAPI NtSetContextThread(
		HANDLE               ThreadHandle,
		PCONTEXT             Context );

	NTSYSAPI NTSTATUS NTAPI LdrLoadDll(
		IN PWCHAR               PathToFile OPTIONAL,
		IN ULONG                Flags OPTIONAL,
		IN PUNICODE_STRING      ModuleFileName,
		OUT PHANDLE             ModuleHandle );
};

void * operator new(size_t size);

void operator delete(void *p);

extern HANDLE g_hProcessHeap;



