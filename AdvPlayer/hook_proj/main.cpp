#include <windows.h>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <D3D9.h>

#include "ilhook.h"
#include "FileReader.h"
#include "FuncHelper.h"
#include "PicSupport.h"
#include "TxtSupport.h"

#include "ws2.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0x44ddd,"\x75\x81","\xb8\xa1") //quote
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

void HOOKFUNC MyCFI(LPLOGFONTW lfi)
{
    //"£Í£Ó ¥´¥·¥Ã¥¯"
    if (wcscmp(lfi->lfFaceName, L"MS UI Gothic") == 0)
    {
        wcscpy_s(lfi->lfFaceName, L"SimHei");
    }
}

void HOOKFUNC MyCW(char** strp)
{
    if (strcmp(*strp, "\x83\x8C\x83\x7E\x83\x6A\x83\x5A\x83\x93\x83\x58") == 0)
    {
        *strp = "Reminiscence";
    }
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        InitWs2();

        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));

        static const HookPointStruct points[] = {
            { nullptr, 0xedaf0, MyOpenFile1, "23", false, 0 },
            { nullptr, 0xedfd8, MyOpenFile, "r", false, 0 },
            { nullptr, 0xE4AE0, MyReadInst, "rf", true, 0x10 },
            { nullptr, 0xcc0bb, MyMbtowc, "r", false, 0 },
            { nullptr, 0xe47be, MySelString, "r", false, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook Ê§°Ü£¡", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectW", MyCFI, "1", false, 0 },
            { "user32.dll", "CreateWindowExA", MyCW, "\x03", false, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 Ê§°Ü£¡", 0, 0);
        }
    }
    return TRUE;
}