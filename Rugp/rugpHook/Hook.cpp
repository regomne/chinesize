#include <Windows.h>
#include "Header.h"
#include <ilhook.h>

//__declspec(naked) void NewCreateFontIndirectA()
//{
//	__asm{
//		mov eax,[esp+4]
//		cmp byte ptr [eax+0x17],80h
//		jne _End
//		mov byte ptr [eax+0x17],86h
//_End:
//		mov eax,OldCreateFontIndirectA
//		jmp eax
//	}
//}

void HOOKFUNC NewCreateFontIndirectA(LPLOGFONTA lf)
{
    if(lf->lfCharSet==0x80)
        lf->lfCharSet=0x86;
}

//__declspec(naked) void NewGetGlyphOutlineA()
//{
//	__asm{
//		mov eax,[esp+8]
//		cmp eax,100h
//		jb _End
//		lea ecx,MapTbl
//		movzx eax,word ptr [ecx+eax*2]
//		test ax,ax
//		jz _End
//		mov [esp+8],eax
//_End:
//		mov eax,OldGetGlyphOutlineA
//		jmp eax
//	}
//}

void HOOKFUNC NewGetGlyphOutlineA(LPDWORD ch)
{
    if(*ch>=0x100 && MapTbl[*ch])
    {
        *ch=MapTbl[*ch];
    }
}

//__declspec(naked) void NewEnumFontFamiliesExA()
//{
//	__asm{
//		mov eax,[esp+8]
//		cmp byte ptr [eax+17h],80h
//		jne _End
//		mov byte ptr [eax+17h],86h
//_End:
//		mov eax,OldEnumFontFamiliesExA
//		jmp eax
//	}
//}

void HOOKFUNC NewEnumFontFamiliesExA(LPLOGFONTA lf)
{
    if(lf->lfCharSet==0x80)
        lf->lfCharSet=0x86;
}

#ifdef MUV_EXTRA
static const char* titleName="マブラヴ - rUGP 6.1";
static const char* newTitleName="Muv-luv";
#else
static const char* titleName="マブラヴ オルタネイティヴ - rUGP 6.1";
static const char* newTitleName="Muv-luv Alternative";
#endif

//__declspec(naked) void NewSetWindowTextA()
//{
//    __asm
//    {
//        push [esp+8]
//        push titleName
//        call dword ptr [lstrcmpA]
//        test eax,eax
//        jnz _End
//        mov eax,newTitleName
//        mov [esp+8],eax
//_End:
//        mov eax,OldSetWindowTextA
//        jmp eax
//    }
//}

void HOOKFUNC NewSetWindowTextA(const char** text)
{
    if(!lstrcmpA(*text,titleName))
        *text=newTitleName;
}

#ifndef MUV_EXTRA
static const char* menuString="チャプターメニューへ戻る";
static const char* newMenuString="ｷｵｻﾘﾕﾂｽﾚｲﾋｵ･(&C)";

// __declspec(naked) void NewAppendMenuA()
// {
//     __asm
//     {
//         push [esp+10h]
//         push menuString
//         call dword ptr [lstrcmpA]
//         test eax,eax
//         jnz _End
//         mov eax,newMenuString
//         mov [esp+10h],eax
// _End:
//         mov eax,OldAppendMenuA
//         jmp eax
//     }
// }
#endif

//__declspec(naked) void NewCreateWindowExA()
//{
//    __asm
//    {
//        push [esp+0ch]
//        push titleName
//        call dword ptr [lstrcmpA]
//        test eax,eax
//        jnz _End
//        mov eax,newTitleName
//        mov [esp+0ch],eax
//_End:
//        mov eax,OldCreateWindowExA
//        jmp eax
//    }
//}

void HOOKFUNC NewCreateWindowExA(const char** text)
{
    if(!lstrcmpA(*text,titleName))
        *text=newTitleName;
}

BOOL InitApiHooks()
{
    BYTE* buff=(BYTE*)VirtualAlloc(0,1000,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

    HookSrcObject src;
    HookStubObject stub;

    HMODULE hm=LoadLibrary(L"gdi32.dll");
    LPVOID func=GetProcAddress(hm,"CreateFontIndirectA");

    if(!InitializeHookSrcObject(&src,func) ||
        !InitializeStubObject(&stub,buff,100) ||
        !Hook32(&src,0,&stub,NewCreateFontIndirectA,"1"))
    {
        return FALSE;
    }

    func=GetProcAddress(hm,"GetGlyphOutlineA");
    if(!InitializeHookSrcObject(&src,func) ||
        !InitializeStubObject(&stub,buff+100,100) ||
        !Hook32(&src,0,&stub,NewGetGlyphOutlineA,"\x02"))
    {
        return FALSE;
    }

    func=GetProcAddress(hm,"EnumFontFamiliesExA");
    if(!InitializeHookSrcObject(&src,func) ||
        !InitializeStubObject(&stub,buff+200,100) ||
        !Hook32(&src,0,&stub,NewEnumFontFamiliesExA,"2"))
    {
        return FALSE;
    }

    hm=LoadLibrary(L"user32.dll");
    func=GetProcAddress(hm,"SetWindowTextA");
    if(!InitializeHookSrcObject(&src,func) ||
        !InitializeStubObject(&stub,buff+300,100) ||
        !Hook32(&src,0,&stub,NewSetWindowTextA,"\x02"))
    {
        return FALSE;
    }
    func=GetProcAddress(hm,"CreateWindowExA");
    if(!InitializeHookSrcObject(&src,func) ||
        !InitializeStubObject(&stub,buff+400,100) ||
        !Hook32(&src,0,&stub,NewCreateWindowExA,"\x03"))
    {
        return FALSE;
    }
#ifndef MUV_EXTRA
//     ret=HookApi(L"user32.dll","AppendMenuA",NewAppendMenuA,&OldAppendMenuA);
//     if(!ret)
//         return ret;
#endif
    return TRUE;
}

int PatchMemory(PatchStruct* strt)
{
	DWORD oldProt;
	BOOL rslt;

	rslt=VirtualProtect((void*)strt->address,strt->len,PAGE_EXECUTE_READWRITE,&oldProt);
	if(!rslt)
	{
		return -1;
	}

	memcpy((void*)strt->address,strt->buff,strt->len);

	return 0;
}