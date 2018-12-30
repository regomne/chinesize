#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "enc.h"
#include "fav.h"

using namespace std;

const char* g_fontName = "SimHei";

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
	DP(nullptr, 0x3281f,nullptr,"\x33") //quote
	DP(nullptr, 0x43b68,nullptr,"\x33") //quote
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

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
		InitializeStringTables();

        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));
        //auto rva = get_ep_rva_from_module_address(GetModuleHandle(nullptr));
        static const HookPointStruct points[] = {
            //{ nullptr, rva, before_start, "r", 0, 0 },
            { nullptr, 0x44b0d, MyProcessString, "r", 0, 0 },
            { nullptr, 0x44b24, MyProcessString, "r", 0, 0 },
            { nullptr, 0x44bc6, MyProcessString, "r", 0, 0 },
            { nullptr, 0x3cb51, MyProcessString2, "r", 0, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook 失败！", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontA", MyCreateFontA, "\x09", 0, 0 },
            { "gdi32.dll", "EnumFontFamiliesExA", MyEnumFontFamliesExA, "2", 0, 0 },
            { "kernel32.dll", "lstrcmpiA", MyLstrcmpiA, "12", STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX, 8 },
            { "user32.dll", "CreateWindowExW", MyCW, "\x03", false, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}