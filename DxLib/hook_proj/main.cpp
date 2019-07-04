#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "enc.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0x226d36, "\x9f", "\xfe") //sjis char code bound
    DP(nullptr, 0x22740a, "\x9f", "\xfe") //sjis char code bound
    DP(nullptr, 0x18acf7, "\x9f", "\xfe") //sjis char code bound
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

void HOOKFUNC MyCW(wchar_t** strp)
{
    if (*strp && wcscmp(*strp, L"想いを捧げる乙女のメロディー") == 0)
    {
        *strp = L"『有少女献鸣的爱之奏章』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

void HOOKFUNC MyGetFileInfo(const wchar_t* fpath)
{
    //auto name = string(fpath);
    LOGERROR("%ws", fpath);
}

void HOOKFUNC MyGetCharcode(Registers* regs)
{
    if (regs->eax == 932)
    {
        regs->eax = 936;
    }
}

typedef int (__cdecl *MyConvStringRoutine)(const char* Src, int SrcStrLength, int SrcCharCodeFormat, char* Dest, size_t BufferBytes, int DestCharCodeFormat);
int HOOKFUNC MyConvString(MyConvStringRoutine oldfunc, const char* Src, int SrcStrLength, int SrcCharCodeFormat, char* Dest, size_t BufferBytes, int DestCharCodeFormat)
{
    if (Src != nullptr && Dest != nullptr && SrcCharCodeFormat == 936 && DestCharCodeFormat == 0x4b0)
    {
        auto src_len = SrcStrLength;
        auto len = ::MultiByteToWideChar(936, 0, Src, src_len, (wchar_t*)Dest, BufferBytes / 2);
        if (src_len == -1)
        {
            return len * 2;
        }
        else
        {
            if (BufferBytes < (len + 1) * 2)
            {
                return -1;
            }
            ((wchar_t*)Dest)[len] = L'\0';
            return (len + 1) * 2;
        }
    }
    return oldfunc(Src, SrcStrLength, SrcCharCodeFormat, Dest, BufferBytes, DestCharCodeFormat);
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        //PatchMemory(g_Patches, ARRAYSIZE(g_Patches));
        //auto rva = get_ep_rva_from_module_address(GetModuleHandle(nullptr));
        static const HookPointStruct points[] = {
            //{ nullptr, rva, before_start, "r", 0, 0 },
            { nullptr, 0x2e33dc, MyGetCharcode, "r", 0, 0 },
            { nullptr, 0x3016e0, MyConvString, "f123456", STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook 失败！", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectA", MyCFI, "1", 0, 0 },
            //{ "user32.dll", "CreateWindowExW", MyCW, "\x03", 0, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}