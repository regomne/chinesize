
#include <windows.h>


typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWCH   Buffer;
} UNICODE_STRING;

struct ProcInfo
{
    LPVOID LdrLoadDllRoutine;
    HANDLE dllHandle;
    UNICODE_STRING dllName;
    WCHAR nameString[20];
};

extern "C"
{
	NTSYSAPI 
		NTSTATUS
		NTAPI
		NtSetContextThread(
		IN HANDLE               ThreadHandle,
		IN CONTEXT*             Context );

	NTSYSAPI 
		NTSTATUS
		NTAPI
		NtGetContextThread(
		IN HANDLE               ThreadHandle,
		OUT CONTEXT*            pContext );
}


void memset1(void* dest,int val,int size)
{
    for(int i=0;i<size;i++)
        ((BYTE*)dest)[i]=val;
}

__declspec(naked) int LoadLib()
{
    __asm{
		pushad
        call lbl
lbl:
        pop ecx
        lea eax,[ecx-0x56+4]
        push eax
        lea eax,[ecx-0x56+8]
        push eax
        xor eax,eax
        push eax
        push eax
        call [ecx-0x56]
        popad
		mov eax,0xffffffff
    }
}

BOOL InjectProcess(HANDLE hp,HANDLE ht,TCHAR* dllName)
{
	BYTE* newMem=(BYTE*)VirtualAllocEx(hp,0,0x100,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if(!newMem)
	{
		return FALSE;
	}

	ProcInfo procInfo;
	procInfo.LdrLoadDllRoutine=GetProcAddress(GetModuleHandle(L"ntdll.dll"),"LdrLoadDll");
	lstrcpy(procInfo.nameString,dllName);
	procInfo.dllName.Buffer=((ProcInfo *)newMem)->nameString;
	procInfo.dllName.Length=lstrlen(dllName)*2;
	procInfo.dllName.MaximumLength=sizeof(procInfo.nameString)*2;

	CONTEXT ctt;
	ctt.ContextFlags=CONTEXT_CONTROL;

	NTSTATUS st=NtGetContextThread(ht,&ctt);
	BYTE* p=(BYTE*)LoadLib;
	BYTE jmpGates[5]={0xe9,0,0,0,0};
	int jmpPos=0;
	for(;p<(BYTE*)LoadLib+0x50;p++)
	{
		if(*p==0xb8 && *(DWORD*)(p+1)==0xffffffff)
		{
			*(DWORD*)&jmpGates[1]=ctt.Eip-(DWORD)(newMem+0x50+(p+5-(BYTE*)LoadLib));
			jmpPos=p-(BYTE*)LoadLib;
			break;
		}
	}
	if(!jmpPos)
		return FALSE;

	DWORD bytesWrote=0;
	BOOL wroteSuccess=TRUE;
	wroteSuccess =wroteSuccess && WriteProcessMemory(hp,newMem,&procInfo,sizeof(procInfo),&bytesWrote);
	wroteSuccess =wroteSuccess && WriteProcessMemory(hp,newMem+0x50,LoadLib,jmpPos,&bytesWrote);
	wroteSuccess =wroteSuccess && WriteProcessMemory(hp,newMem+0x50+jmpPos,jmpGates,5,&bytesWrote);

	if(!wroteSuccess)
		return FALSE;

	ctt.Eip=(DWORD)(newMem+0x50);
	st=NtSetContextThread(ht,&ctt);
	return TRUE;
}

BOOL CreateAndInject(TCHAR* appName, TCHAR* dllName)
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    if(lstrlen(dllName)>=20)
        return FALSE;

    memset1(&si,0,sizeof(si));
    si.cb=sizeof(si);

    if(GetFileAttributes(appName)==-1)
        return FALSE;

    if(!CreateProcess(0,appName,0,0,FALSE,CREATE_SUSPENDED,0,0,&si,&pi))
        return FALSE;

	if(!InjectProcess(pi.hProcess,pi.hThread,dllName))
	{
		TerminateProcess(pi.hProcess,0);
		return FALSE;
	}

    ResumeThread(pi.hThread);

    return TRUE;
}