
#include <windows.h>

#include <ilhook.h>
#include <v8.h>
#include "favhook.h"

using namespace v8;

HANDLE QueryEvent;
HANDLE RsltEvent;
BOOL v8Ready;
DWORD QueryOffset;
char* FoundText;
char _foundText[0x1000];

typedef int (WINAPI *D3dCreateRoutine)(UINT);
D3dCreateRoutine CreateFunc;

Handle<Context> InitV8();

char* FindTextByOffset(DWORD off)
{
    if(v8Ready)
    {
        QueryOffset=off;
        SetEvent(QueryEvent);
        DWORD ret=WaitForSingleObject(RsltEvent,1000);
        if(ret==WAIT_OBJECT_0)
            return FoundText;
    }
	return 0;
}

void WINAPI MyProcessString(ScriptObject* obj,char** str)
{
	BYTE* ps=(BYTE*)*str;
	if(ps[-2]!=0xe || ps[ps[-1]-1]!=0)
		return;

	char* ns=FindTextByOffset(ps-1-(BYTE*)obj->ScriptBuffer);
	if(ns)
		*str=ns;
}

void WINAPI MyProcessString2(BYTE* s,char** str)
{
	ScriptObject* obj=(ScriptObject*)(s+0x643f18);
	MyProcessString(obj,str);
}

void WINAPI MyCreateFontA(DWORD* charset)
{
	*charset=0x86;
}
void WINAPI MyEnumFontFamliesExA(LPLOGFONTA lpf)
{
	lpf->lfCharSet=0x86;
}

void WINAPI MyCreateWindowExA(char** title)
{
    if(*(DWORD*)*title==0xeb82a282)
    {
        wchar_t* name=L"五彩斑斓的曙光 -the eternal chapter in world's end tale.- ver1.1";
        static char mb[100];

        int ret=WideCharToMultiByte(CP_ACP,0,name,-1,mb,100,0,0);

        *title=mb;
    }
}

int WINAPI MyLstrcmpiA(char* s1,char* s2)
{
	return CompareStringA(0x411,NORM_IGNORECASE,s1,-1,s2,-1) -2;
}

BOOL PatchMemory(void* addr,void* content,int length)
{
	DWORD oldProt;
	if(!VirtualProtect(addr,length,PAGE_EXECUTE_READWRITE,&oldProt))
		return FALSE;
	memcpy(addr,content,length);
	return TRUE;
}

BOOL PatchAndHook()
{
    auto stubMem=(BYTE*)VirtualAlloc(0,0x1000,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(!stubMem)
    {
        MessageBox(0,L"似乎没内存了",0,0);
        return FALSE;
    }

    HookSrcObject srcObj;
    HookStubObject stubObj;

    if(!InitializeHookSrcObject(&srcObj,(void*)0x442b4d) ||
        !InitializeStubObject(&stubObj,stubMem,0x1000) ||
        !Hook32(&srcObj,0,&stubObj,MyProcessString,"y\x01"))
    {
        MessageBox(0,L"无法hook函数1",0,0);
        return FALSE;
    }
    if(!InitializeHookSrcObject(&srcObj,(void*)0x442b64) ||
        !InitializeStubObject(&stubObj,stubMem+100,0x1000-100) ||
        !Hook32(&srcObj,0,&stubObj,MyProcessString,"y\x01"))
    {
        MessageBox(0,L"无法hook函数2",0,0);
        return FALSE;
    }
    if(!InitializeHookSrcObject(&srcObj,(void*)0x442c06) ||
        !InitializeStubObject(&stubObj,stubMem+200,0x1000-200) ||
        !Hook32(&srcObj,0,&stubObj,MyProcessString,"y\x01"))
    {
        MessageBox(0,L"无法hook函数3",0,0);
        return FALSE;
    }
    if(!InitializeHookSrcObject(&srcObj,(void*)0x43ad31) ||
        !InitializeStubObject(&stubObj,stubMem+300,0x1000-300) ||
        !Hook32(&srcObj,0,&stubObj,MyProcessString2,"b\x01"))
    {
        MessageBox(0,L"无法hook函数4",0,0);
        return FALSE;
    }

    return TRUE;
}

BOOL PatchAndHook0()
{
#ifdef DBG1
#define DBGP(x) printf(x);
#else
#define DBGP(x)
#endif
    HookSrcObject srcObj;
    HookStubObject stubObj;

	DBGP("allocing...");
    auto stubMem=(BYTE*)VirtualAlloc(0,500,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(!stubMem)
    {
        MessageBox(0,L"似乎没内存了",0,0);
        return FALSE;
    }

	DBGP("patching cwe...");
	HMODULE hm=LoadLibrary(L"user32.dll");
	DBGP("getproc");
    auto func=GetProcAddress(hm,"CreateWindowExA");
	DBGP("init and hook");
    if(!InitializeHookSrcObject(&srcObj,func) ||
        !InitializeStubObject(&stubObj,stubMem,100) ||
        !Hook32(&srcObj,0,&stubObj,MyCreateWindowExA,"\x03"))
    {
        MessageBox(0,L"无法hook CreateWindowExA",0,0);
        return FALSE;
    }

	DBGP("patching eff...");
	hm=LoadLibrary(L"gdi32.dll");
	func=GetProcAddress(hm,"EnumFontFamiliesExA");
	if(!InitializeHookSrcObject(&srcObj,func) ||
		!InitializeStubObject(&stubObj,stubMem+100,100) ||
		!Hook32(&srcObj,0,&stubObj,MyEnumFontFamliesExA,"2"))
	{
		MessageBox(0,L"无法hook eff",0,0);
		return FALSE;
	}
	DBGP("patching cf...");
	func=GetProcAddress(hm,"CreateFontA");
	if(!InitializeHookSrcObject(&srcObj,func) ||
		!InitializeStubObject(&stubObj,stubMem+200,100) ||
		!Hook32(&srcObj,0,&stubObj,MyCreateFontA,"\x09"))
	{
		MessageBox(0,L"无法hook cf",0,0);
		return FALSE;
	}

	DBGP("patching lstr...");
	hm=LoadLibrary(L"kernel32.dll");
	func=GetProcAddress(hm,"lstrcmpiA");
	if(!InitializeHookSrcObject(&srcObj,func) ||
		!InitializeStubObject(&stubObj,stubMem+300,100,8,STUB_DIRECTLYRETURN|STUB_OVERRIDEEAX) ||
		!Hook32(&srcObj,0,&stubObj,MyLstrcmpiA,"12"))
	{
		MessageBox(0,L"无法hook lstrcmpi",0,0);
		return FALSE;
	}

	DBGP("patching exe...")
	PatchMemory((void*)0x430dbf,"\x33",1);
	PatchMemory((void*)0x441bd8,"\x33",1);
	char* fontName="SimHei";
	PatchMemory((void*)0x43f268,&fontName,4);

    return TRUE;
}

int WINAPI Working(HANDLE th)
{
    //SuspendThread(th);

#ifdef DBG1
	AllocConsole();
	_wfreopen(L"CONOUT$", L"w", stdout);
	_wfreopen(L"CONOUT$", L"w", stderr);
	_wfreopen(L"CONIN$", L"r", stdin);

	printf("Begin patching...");
#endif
//    if(!PatchAndHook0())
//        ExitProcess(0);
//    if(ResumeThread(th)==-1)
//	{
//#ifdef DBG1
//		printf("err resume: %d",GetLastError());
//#else
//		MessageBox(0,L"启动失败，请重新启动游戏",0,0);
//		ExitProcess(0);
//#endif
//	}


    auto isolate=Isolate::New();
    isolate->Enter();

    HandleScope handle_scope(isolate);

    auto context=InitV8();
    context->Enter();

#ifdef DBG1
    HANDLE hf2=CreateFile(L"d:/ok.js",GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hf2!=INVALID_HANDLE_VALUE)
	{
		DWORD temp;
		auto jsbuff=new BYTE[100000];
		ReadFile(hf2,jsbuff,100000,&temp,0);
		CloseHandle(hf2);
		auto src=String::NewFromUtf8(isolate,(char*)jsbuff,String::kNormalString,temp);
		auto fname=String::NewFromUtf8(isolate,"wohehe");
		auto script=Script::Compile(src,fname);
		script->Run();
	}
#endif

    auto global=context->Global();
	auto ReadText=global->Get(String::NewFromUtf8(isolate,"rt")).As<Function>();
	auto GetTextByOffset=global->Get(String::NewFromUtf8(isolate,"gtfo")).As<Function>();
    auto AddTable=global->Get(String::NewFromUtf8(isolate,"aa")).As<Function>();
    auto addrTbl=Array::New(isolate,0);

	HANDLE hf=CreateFileW(L"cnpack",GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hf==INVALID_HANDLE_VALUE)
	{
		MessageBox(0,L"无法打开cnpack",0,0);
		ExitProcess(0);
	}
    Handle<Value> arg[4];
	arg[0]=Integer::NewFromUnsigned(isolate,(uint32_t)hf);
	auto texts=ReadText->Call(global,1,arg).As<Object>();

	auto cc=global->Get(String::NewFromUtf8(isolate,"CryptoJS"));
	if(cc.IsEmpty())
	{
		MessageBox(0,L"cnpack可能已损坏！",0,0);
		ExitProcess(0);
	}
	CloseHandle(hf);
    v8Ready=TRUE;


	//SuspendThread(th);
	//if(ResumeThread(th)==-1)
	//{
	//	printf("%d\n",GetLastError());
	//}

	while(TRUE)
	{
		DWORD ret=WaitForSingleObject(QueryEvent,INFINITE);
		if(ret==WAIT_OBJECT_0)
		{
			HandleScope scope(isolate);
			Handle<Value> arg[3];
			arg[0]=Integer::NewFromUnsigned(isolate,QueryOffset);
			arg[1]=texts;
			arg[2]=addrTbl;
			auto text=GetTextByOffset->Call(global,3,arg);
			if(text.IsEmpty() || !text->IsString())
				FoundText=0;
			else
			{
				text.As<String>()->WriteOneByte((uint8_t*)_foundText);
				FoundText=_foundText;
			}
			SetEvent(RsltEvent);
		}
		else
		{
			Sleep(10);
		}
	}

    //context->Exit();
    return 0;
}

int WINAPI DllMain( HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved )
{
	if(dwReason==DLL_PROCESS_ATTACH)
	{
		wchar_t systemDir[100];
		GetSystemDirectory(systemDir,100);
		wcscat(systemDir,L"\\d3d9.dll");
		HMODULE hm=LoadLibrary(systemDir);
		CreateFunc=(D3dCreateRoutine)GetProcAddress(hm,"Direct3DCreate9");

		//HANDLE th;
		//DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&th,0,FALSE,
		//	DUPLICATE_SAME_ACCESS);

		if(!PatchAndHook0())
			ExitProcess(0);
        if(!PatchAndHook())
        {
            ExitProcess(0);
        }

        QueryEvent=CreateEvent(0,FALSE,FALSE,0);
        RsltEvent=CreateEvent(0,FALSE,FALSE,0);

        CreateThread(0,0,(LPTHREAD_START_ROUTINE)Working,0,0,0);
	}
	return TRUE;
}

int WINAPI Direct3DCreate9(UINT ver)
{
	return CreateFunc(ver);
}