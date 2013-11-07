#pragma once

#include <Windows.h>
#include "fxckBGI.h"

struct GameInfo{
	WCHAR* gameName;
	DWORD funcAddress;
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


int InjectToProcess(HANDLE process,HANDLE thread,wchar_t* dllPath,DecoprFunc func);
int PipeComm(HANDLE,DWORD val);