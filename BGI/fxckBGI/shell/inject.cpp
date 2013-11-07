#include <windows.h>
#include "shell.h"
#include "fxckBGI.h"

#define CODE_OFFSET 1024

__declspec(naked) int LoadLib()
{
	__asm{
		//_emit 0xcc
		pushad
		call lbl
		lbl:
		pop ebx
		sub ebx,CODE_OFFSET+6
		
		lea eax,[ebx+4]
		push eax
		lea eax,[ebx+8]
		push eax
		xor eax,eax
		push eax
		push eax
		call [ebx]
		popad
		_emit 0xff
		_emit 0xff
		_emit 0xff
		_emit 0xff
		_emit 0xff
	}
	
}
int InjectToProcess(HANDLE process,HANDLE thread,wchar_t* dllPath,DecoprFunc func)
{
	BYTE* CodeAddr=(BYTE*)VirtualAllocEx(process,0,4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if(!CodeAddr)
		return -1;
	DWORD strLen=lstrlen(dllPath);
	PWCH StringAddr=(PWCH)VirtualAllocEx(process,0,(strLen+1)*2,MEM_COMMIT,PAGE_READWRITE);
	if(!StringAddr)
		return -1;

	MyDataStruct mds;
	
	mds.dllName.Buffer=StringAddr;
	mds.dllName.Length=(WORD)(strLen*2);
	mds.dllName.MaximumLength=(strLen+1)*2;

	mds.funcAddr=func;
	mds.loadDll=LdrLoadDll;

	int jmpPos;
	BYTE* p=(BYTE*)LoadLib;
	for(int i=0;;i++)
	{
		if(p[i]==0xff && *((DWORD*)(&p[i+1]))==0xffffffff)
		{
			jmpPos=i;
			break;
		}
	}

	CONTEXT ctt;
	ctt.ContextFlags=CONTEXT_INTEGER|CONTEXT_CONTROL;
	NTSTATUS val=NtGetContextThread(thread,&ctt);
	if(!NT_SUCCESS(val))
		return -1;

	DWORD readBytes;
	BYTE inst[8];
	BOOL rslt=WriteProcessMemory(process,CodeAddr,&mds,sizeof(mds),&readBytes);
	rslt=rslt&&WriteProcessMemory(process,CodeAddr+CODE_OFFSET,LoadLib,jmpPos,&readBytes);
	inst[0]=0xe9;
	*(DWORD*)(&inst[1])=ctt.Eip-((DWORD)CodeAddr+CODE_OFFSET+jmpPos+5);
	rslt=rslt&&WriteProcessMemory(process,CodeAddr+CODE_OFFSET+jmpPos,inst,5,&readBytes);
	rslt=rslt&&WriteProcessMemory(process,StringAddr,dllPath,strLen*2,&readBytes);

	if(!rslt)
		return -1;

	ctt.Eip=(DWORD)CodeAddr+CODE_OFFSET;
	if(!NT_SUCCESS(NtSetContextThread(thread,&ctt)))
		return -1;

	return 0;
}

int PipeComm(HANDLE pipe,DWORD val)
{
	BOOL fConnected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
	if(!fConnected)
	{
		return -1;
	}

	DWORD readBytes;
	WriteFile(pipe,&val,4,&readBytes,0);
	if(readBytes!=4)
	{
		DisconnectNamedPipe(pipe);
		return -1;
	}

	ReadFile(pipe,&val,4,&readBytes,0);

	if(val!='OVER')
	{
		DisconnectNamedPipe(pipe);
		return -1;
	}
	DisconnectNamedPipe(pipe);
	return 0;
}