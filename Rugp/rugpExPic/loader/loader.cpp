#include <Windows.h>
#include "loader.h"

#pragma comment(lib,"ntdll.lib")
#pragma comment(linker,"/entry:main")
#pragma comment(linker,"/nodefaultlib")

#define CODE_OFFSET 1024

HANDLE g_hInstance;
HANDLE g_hProcessHeap;

void * operator new(size_t size)
{
	void *p = HeapAlloc(g_hProcessHeap,0,size);
	return (p);
}

void operator delete(void *p)
{
	HeapFree(g_hProcessHeap,0,p);
}

void memset1(void* src, unsigned char val, unsigned int count)
{
	unsigned char* p=(unsigned char*)src;
	for(unsigned int i=0;i<count;i++)
		p[i]=val;
}


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
int InjectToProcess(HANDLE process,HANDLE thread,wchar_t* dllPath)
{
	BYTE* CodeAddr=(BYTE*)VirtualAllocEx(process,0,4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if(!CodeAddr)
		return -1;
	DWORD strLen=lstrlen(dllPath);
	if(strLen>32767)
		return -1;
	PWCH StringAddr=(PWCH)VirtualAllocEx(process,0,(strLen+1)*2,MEM_COMMIT,PAGE_READWRITE);
	if(!StringAddr)
		return -1;

	MyDataStruct mds;
	
	mds.dllName.Buffer=StringAddr;
	mds.dllName.Length=(WORD)(strLen*2);
	mds.dllName.MaximumLength=(strLen+1)*2;

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

int main1()
{
	if(GetFileAttributes(L"extract.dll")==-1)
	{
		MessageBox(0,L"Can't find extract.dll!",0,MB_ICONERROR);
		return 0;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	memset1(&si,0,sizeof(si));
	si.cb=sizeof(si);

	wchar_t exePath[]=L"rugp.exe ;PF:UGPAPP={age\\マブラヴオルタネイティヴLatest}";
	int rslt=CreateProcess(0,exePath,0,0,FALSE,CREATE_SUSPENDED,
		0,0,&si,&pi);

	if(!rslt)
	{
		MessageBox(0,L"无法创建进程",0,0);
		return 0;
	}

	int pathLen=256;
	wchar_t* dllPath=new wchar_t[pathLen];
	int retlen=GetModuleFileName(0,dllPath,pathLen);
	while(GetLastError()==ERROR_INSUFFICIENT_BUFFER)
	{
		delete[] dllPath;
		pathLen*=2;
		dllPath=new wchar_t[pathLen];
		retlen=GetModuleFileName(0,dllPath,pathLen);
	};
	wchar_t* p=dllPath+retlen;
	for(;p>dllPath;p--)
		if(*p==L'\\')
			break;
	*(p+1)=L'\0';
	lstrcat(dllPath,L"extract.dll");

	rslt=InjectToProcess(pi.hProcess,pi.hThread,dllPath);
	if(rslt!=0)
	{
		MessageBox(0,L"无法注入进程",0,0);
		return 0;
	}

	ResumeThread(pi.hThread);

	return 0;
}

int main()
{
	g_hInstance=GetModuleHandle(0);
	g_hProcessHeap=GetProcessHeap();
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	main1();
	CoUninitialize();
	ExitProcess(0);
}