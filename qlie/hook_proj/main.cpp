#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "enc.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    /*
    .text:004F06F6 80 3D 1C 94 70 00 00                    cmp     byte_70941C, 0
    .text:004F06FD 75 4C                                   jnz     short loc_4F074B
    .text:004F06FF
    .text:004F06FF                         loc_4F06FF:                             ; CODE XREF: sub_4F05C4+130↑j
    .text:004F06FF 8D 55 EC                                lea     edx, [ebp+var_14]
    .text:004F0702 8B C7                                   mov     eax, edi
    .text:004F0704 E8 FB 62 F2 FF                          call    sub_416A04
    .text:004F0709 8B 45 EC                                mov     eax, [ebp+var_14]
    .text:004F070C 8D 55 F4                                lea     edx, [ebp+var_C]
    .text:004F070F E8 74 45 F2 FF                          call    sub_414C88
    .text:004F0714 8B 45 F4                                mov     eax, [ebp+var_C]
    .text:004F0717 BA A8 07 4F 00                          mov     edx, offset aS_1 ; ".s"
    .text:004F071C E8 DF 75 F1 FF                          call    sub_407D00
    .text:004F0721 74 28                                   jz      short loc_4F074B
    */
    DP(nullptr, 0xf0721,"\x74\x28","\x90\x90") // ".s" compare1
    DP(nullptr, 0xf06fd,"\x75\x4C","\x90\x90") // a real file flag in the same function with ".s"-compare1

    /*
    .text:004F085B 8D 55 E8                                lea     edx, [ebp+var_18]
    .text:004F085E 8B C3                                   mov     eax, ebx
    .text:004F0860 E8 9F 61 F2 FF                          call    sub_416A04
    .text:004F0865 8B 45 E8                                mov     eax, [ebp+var_18]
    .text:004F0868 8D 55 EC                                lea     edx, [ebp+var_14]
    .text:004F086B E8 18 44 F2 FF                          call    sub_414C88
    .text:004F0870 8B 45 EC                                mov     eax, [ebp+var_14]
    .text:004F0873 BA B4 09 4F 00                          mov     edx, offset aS_2 ; ".s"
    .text:004F0878 E8 83 74 F1 FF                          call    sub_407D00
    .text:004F087D 0F 84 F2 00 00 00                       jz      loc_4F0975
    */
    DP(nullptr, 0xf087d,"\x0F\x84\xF2\x00\x00\x00","\x90\x90\x90\x90\x90\x90") // ".s" compare2
};
#undef DP

__declspec(naked) void D3dCreate9()
{
    __asm jmp Direct3DCreate9
}

bool IsHalf(const string& s)
{
    for (auto c : s)
    {
        if ((unsigned char)c >= 0x80)
        {
            return false;
        }
    }
    return true;
}

void HOOKFUNC MyCFI(LPLOGFONTA lfi)
{
	lfi->lfCharSet = 0x86;
    //"ＭＳ ゴシック"
    //if (wcscmp(lfi->lfFaceName, L"MS UI Gothic") == 0)
    //{
    //    wcscpy_s(lfi->lfFaceName, L"SimHei");
    //}
}

void HOOKFUNC MyEFF(LPLOGFONTW lfi)
{
	lfi->lfCharSet = 0x86;
}

void HOOKFUNC MyCW(wchar_t** strp)
{
    if (*strp && wcscmp(*strp, L"想いを捧げる乙女のメロディー") == 0)
    {
        *strp = L"『有少女献鸣的爱之奏章』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

void HOOKFUNC MyWCTM(DWORD* esp, int cp, wchar_t* ori)
{
    LOGERROR(L"W: %s", ori);
    if (wcsstr(ori, L"音楽") != nullptr)
    {
        wchar_t m[100];
        wprintf_s(m, "addr:%p", *esp);
        MessageBox(nullptr, m, nullptr, 0);
    }
}

typedef int(__stdcall *mbcs_routine)(
    UINT                              CodePage,
    DWORD                             dwFlags,
    LPCCH                             lpMultiByteStr,
    int                               cbMultiByte,
    LPWSTR                            lpWideCharStr,
    int                               cchWideChar
    );

int HOOKFUNC MyMBCS(
    mbcs_routine                      old_func,
    UINT                              CodePage,
    DWORD                             dwFlags,
    LPCCH                             lpMultiByteStr,
    int                               cbMultiByte,
    LPWSTR                            lpWideCharStr,
    int                               cchWideChar
) {
    if (CodePage == 0)
    {
        if ((cbMultiByte > 3 || cbMultiByte == -1 && strlen(lpMultiByteStr) > 3) && memcmp(lpMultiByteStr, "\xef\xbb\xbf", 3) == 0) {
            CodePage == CP_UTF8;
            lpMultiByteStr += 3;
            if (cbMultiByte != -1) cbMultiByte -= 3;
        } else {
            CodePage = 932;
        }
    }
    return old_func(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);

    if (cbMultiByte == -1)
    {
        if (strstr(lpMultiByteStr, "\x89\xb9\x8a\x79") != nullptr)
        {
            printf("aaa");
        }
    }
    else
    {
        auto s2 = string(lpMultiByteStr, cbMultiByte);
        if (s2.find("\x89\xb9\x8a\x79") != string::npos)
        {
            printf("bbb");
        }
    }
    static bool entered = false;
    auto ret = old_func(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
    return ret;
    if (!entered && ret > 0 && cchWideChar > 0)
    {
        entered = true;
        if (cchWideChar >= 4090)
            cchWideChar = 4090;
        if (ret >= cchWideChar)
        {
            ret = cchWideChar - 1;
        }
        wchar_t old_char = lpWideCharStr[ret];
        lpWideCharStr[ret] = 0;
        LOGERROR(L"M: %s", lpWideCharStr);
        lpWideCharStr[ret] = old_char;
        entered = false;
    }
    return ret;
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));
        //auto rva = get_ep_rva_from_module_address(GetModuleHandle(nullptr));
        //static const HookPointStruct points[] = {
        //    { nullptr, rva, before_start, "r", 0, 0 },
        //    //{ nullptr, 0x2F60, my_read_pic, "rf", STUB_JMP_EAX_AFTER_RETURN, 0 },
        //};

        //if (!HookFunctions(points))
        //{
        //    MessageBox(0, L"Hook 失败！", 0, 0);
        //    return TRUE;
        //}

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectA", MyCFI, "1", 0, 0 },
            { "gdi32.dll", "EnumFontFamiliesExW", MyEFF, "2", 0, 0 },
            //{ "user32.dll", "CreateWindowExW", MyCW, "\x03", 0, 0 },
            //{ "kernel32.dll", "WideCharToMultiByte", MyWCTM, "w13", 0, 0 },
            //{ "kernel32.dll", "MultiByteToWideChar", MyMBCS, "f123456", STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX, 24 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}