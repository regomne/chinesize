#include <windows.h>
#include "Regomne.h"
#include "resource.h"

#pragma comment(linker,"/entry:WinMain0")

//wchar_t globalExeName[]=L"D:\\Program Files\\女装山脈\\女装山脈.exe";
wchar_t globalExeName[]=L"女装山脈.exe";

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
		IN UNICODE_STRING*      ModuleFileName,
		OUT PHANDLE             ModuleHandle );
};
void ReadCharTbl(wchar_t*);

extern "C"
{
	extern BYTE CodeStart;
	extern BYTE CodeEnd;
	extern BYTE SetSEH;
};

struct MyDataStruct{
	BYTE* hookApi;
	BYTE* loadDll;
	BYTE* tbl;
	BYTE* fontResc;
	ULONG fontSize;
};

int __declspec(naked) __stdcall HookWin32API(BYTE* addr,BYTE* newAddr)
{
	__asm{
		push ebp
		mov ebp,esp
	}
	if(*(WORD*)addr==0xff8b && *(DWORD*)(addr-5)==0x90909090)
	{
		*(WORD*)addr=0xf9eb;
		*(DWORD*)(addr-4)=newAddr-addr;
		*(addr-5)=0xe9;
	}
	__asm{
		pop ebp
		ret 8
	}
}

int __declspec(naked) NewCreateFont()
{
	__asm{
		mov dword ptr [esp+24h],86h
		mov eax,47b084h
		mov ecx,[eax]
		add ecx,2
		jmp ecx
	}
}

void DecFont(BYTE* readme,DWORD size,BYTE* fontData)
{
	for(DWORD i=0;i<size;i++)
	{
		*fontData++^=*readme++;
	}
}

int WINAPI WinMain1()
{
	//if(GetFileAttributes(globalExeName)==-1)
	//{
	//	MessageBox(0,L"Can't find 夏空カナタ.exe!",L"Loader",0);
	//	return 0;
	//}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	BOOL rslt=CreateProcess(0,globalExeName,0,0,FALSE,CREATE_SUSPENDED,0,0,&si,&pi);
	if(!rslt)
	{
		MessageBox(0,L"Can't start the process!",L"Loader",0);
		return 0;
	}

	BYTE* CodeAddr=(BYTE*)VirtualAllocEx(pi.hProcess,0,2560000,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if(!CodeAddr)
	{
		MessageBox(0,L"Can't allocate memory!",L"Loader",0);
		return 0;
	}
	DWORD temp;
	rslt=VirtualProtect(&CodeStart,10,PAGE_EXECUTE_READWRITE,&temp);
	MyDataStruct* MyData=(MyDataStruct*)&CodeStart;
	
	wchar_t* charTbl;
	HMODULE hmod=GetModuleHandle(0);
	HRSRC res=FindResource(hmod,(LPCWSTR)IDR_2471,(LPCWSTR)247);
	charTbl=(wchar_t*)LockResource(LoadResource(hmod,res));
	res=FindResource(hmod,(LPCWSTR)IDR_2472,(LPCWSTR)247);
	int myFontsize=SizeofResource(hmod,res);
	BYTE* myFont;
	myFont=(BYTE*)LockResource(LoadResource(hmod,res));
	BYTE* decFont=new BYTE[myFontsize];
	memcpy(decFont,myFont,myFontsize);

	if(GetFileAttributes(L"luozhang.pac")!=-1)
	{
		if(MessageBox(0,L"即将删除1.0补丁文件，要继续吗？\n如果不删除，请在下次启动游戏前将luozhang.pac改名",L"Loader",MB_YESNO)==IDYES)
		{
			DeleteFile(L"luozhang.pac");
			DeleteFile(L"女装山脈cn1.0.exe");
		}
	}
	
	HANDLE hreadme=CreateFile(L"readme.ini",GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hreadme!=INVALID_HANDLE_VALUE)
	{
		DWORD temp;
		DWORD rSize=GetFileSize(hreadme,0);
		BYTE* rBuff=new BYTE[rSize];
		ReadFile(hreadme,rBuff,rSize,&temp,0);
		DecFont(rBuff,rSize,decFont);
		CloseHandle(hreadme);
	}
	else
	{
		MessageBox(0,L"未找到readme.ini文件，这可能会导致游戏文本乱码！",L"Loader",0);
	}

//	ReadCharTbl(charTbl);

	MyData->hookApi=CodeAddr;
	MyData->loadDll=(BYTE*)LdrLoadDll;
	MyData->tbl=CodeAddr+5000;
	MyData->fontResc=CodeAddr+1000000;
	MyData->fontSize=myFontsize;
	
	rslt=WriteProcessMemory(pi.hProcess,CodeAddr,HookWin32API,100,&temp);
	rslt=WriteProcessMemory(pi.hProcess,CodeAddr+200,&CodeStart,(&CodeEnd-&CodeStart),&temp);
	rslt=WriteProcessMemory(pi.hProcess,CodeAddr+5000,charTbl,65536*sizeof(wchar_t),&temp);
	rslt=WriteProcessMemory(pi.hProcess,CodeAddr+1000000,decFont,myFontsize,&temp);

	CONTEXT ctt;
	ctt.ContextFlags=CONTEXT_INTEGER|CONTEXT_CONTROL;
	NTSTATUS val=NtGetContextThread(pi.hThread,&ctt);
	ctt.Ecx=ctt.Eip;
	ctt.Eip=(DWORD)(&SetSEH-&CodeStart+(CodeAddr+200));
	NtSetContextThread(pi.hThread,&ctt);

	ResumeThread(pi.hThread);
	CloseHandle(pi.hProcess);

	return 0;
}

int WinMain0()
{
	WinMain1();
	ExitProcess(0);
}
