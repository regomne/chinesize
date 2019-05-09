#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "enc.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0xEC45D,"\x74\x28","\x90\x90") // ".s" compare1
    DP(nullptr, 0xEC5B9,"\x0F\x84\xF2\x00\x00\x00","\x90\x90\x90\x90\x90\x90") // ".s" compare2
    DP(nullptr, 0xEC439,"\x75\x4C","\x90\x90") // a real file flag in the same function with ".s"-compare1
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
            { "kernel32.dll", "MultiByteToWideChar", MyMBCS, "f123456", STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX, 24 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}