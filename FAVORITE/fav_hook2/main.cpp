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

void HOOKFUNC MyCW(char** strp)
{
    if (*strp && strcmp(*strp, "\x82\xB3\x82\xAD\x82\xE7\x81\x41\x82\xE0\x82\xE4\x81\x42\x20\x2D\x61\x73\x20\x74\x68\x65\x20\x4E\x69\x67\x68\x74\x27\x73\x2C\x20\x52\x65\x69\x6E\x63\x61\x72\x6E\x61\x74\x69\x6F\x6E\x2D\x20\x91\xCC\x8C\xB1\x94\xC5") == 0)
    {
        *strp = "『樱、萌芽』中文体验版";
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
            { "user32.dll", "CreateWindowExA", MyCW, "\x03", false, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}