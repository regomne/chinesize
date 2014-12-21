#include <windows.h>
#include "ilhook.h"

#pragma comment(linker,"/entry:DllMain")

BOOL InjectProcess(HANDLE hp,HANDLE ht,TCHAR* dllName);


typedef BOOL (WINAPI *CreateProcessWRoutine)(
	_In_opt_     LPCTSTR lpApplicationName,
	_Inout_opt_  LPTSTR lpCommandLine,
	_In_opt_     LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_         BOOL bInheritHandles,
	_In_         DWORD dwCreationFlags,
	_In_opt_     LPVOID lpEnvironment,
	_In_opt_     LPCTSTR lpCurrentDirectory,
	_In_         LPSTARTUPINFO lpStartupInfo,
	_Out_        LPPROCESS_INFORMATION lpProcessInformation
	);


BOOL WINAPI MyCreateProcessW(
	CreateProcessWRoutine func,
	_In_opt_     LPCTSTR lpApplicationName,
	_Inout_opt_  LPTSTR lpCommandLine,
	_In_opt_     LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_         BOOL bInheritHandles,
	_In_         DWORD dwCreationFlags,
	_In_opt_     LPVOID lpEnvironment,
	_In_opt_     LPCTSTR lpCurrentDirectory,
	_In_         LPSTARTUPINFO lpStartupInfo,
	_Out_        LPPROCESS_INFORMATION lpProcessInformation
	)
{
	int hasSus= (dwCreationFlags & CREATE_SUSPENDED);
	dwCreationFlags |= CREATE_SUSPENDED;
	BOOL ret=func(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,
		dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
	if(ret)
	{
		InjectProcess(lpProcessInformation->hProcess,lpProcessInformation->hThread,L"fucktpm.dll");
	}
	if(!hasSus)
		ResumeThread(lpProcessInformation->hThread);
	return ret;
}

char newStr[1000];

void WINAPI MyCreateFileA(char** pfname)
{
    auto fname=*pfname;
    int len=strlen(fname);
    if(len<10)
        return;

    auto p=strrchr(fname,'\\');
    if(p==0)
        return;
    p=strrchr(p,'.');
    if(p==0)
        return;

    memcpy(newStr,fname,(p-fname+1));
    strcat(newStr,"old");

    if(GetFileAttributesA(newStr)!=-1)
    {
        *pfname=newStr;
    }
}

int WINAPI MyGetSysWow64A(char* buffer,DWORD size)
{
    return GetSystemDirectoryA(buffer,size);
}

int WINAPI DllMain( HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved )
{
    if(dwReason==DLL_PROCESS_ATTACH)
    {
        BYTE* buff=(BYTE*)VirtualAlloc(0,1000,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

        HookSrcObject src;
        HookStubObject stub;


		auto hm=LoadLibrary(L"kernel32.dll");
		auto func=GetProcAddress(hm,"CreateProcessW");
		if(!InitializeHookSrcObject(&src,func) ||
			!InitializeStubObject(&stub,buff,100,40,STUB_DIRECTLYRETURN|STUB_OVERRIDEEAX) ||
			!Hook32(&src,0,&stub,MyCreateProcessW,"f123456789A"))
		{
			MessageBox(0,L"无法hook函数4",0,0);
			return FALSE;
		}

        //func=GetProcAddress(hm,"CreateFileA");
        //if(!InitializeHookSrcObject(&src,func) ||
        //    !InitializeStubObject(&stub,buff+100,100) ||
        //    !Hook32(&src,0,&stub,MyCreateFileA,"\x01"))
        //{
        //    MessageBox(0,L"无法hook函数1",0,0);
        //    return FALSE;
        //}

        func=GetProcAddress(hm,"GetSystemWow64DirectoryA");
        if(func)
        {
            if(!InitializeHookSrcObject(&src,func) ||
                !InitializeStubObject(&stub,buff+200,100,8,STUB_DIRECTLYRETURN|STUB_OVERRIDEEAX) ||
                !Hook32(&src,0,&stub,MyGetSysWow64A,"12"))
            {
                MessageBox(0,L"无法hook函数21",0,0);
                return FALSE;
            }
        }
    }
    return TRUE;
}