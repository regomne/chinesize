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
    //"ＭＳ ゴシック"
    if (wcscmp(lfi->lfFaceName, L"MS UI Gothic") == 0 ||
        wcscmp(lfi->lfFaceName, L"ＭＳ ゴシック") == 0)
    {
        wcscpy_s(lfi->lfFaceName, L"SimHei");
    }
}

void HOOKFUNC MyCW(wchar_t** strp)
{
    if (*strp && wcscmp(*strp, L"乙女が彩る恋のエッセンス") == 0)
    {
        *strp = L"『有少女涂彩的爱之芬芳』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        InitWs2();

        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));

        static const HookPointStruct points[] = {
            { nullptr, 0xE9740, MyOpenFile1, "23", false, 0 },
            { nullptr, 0xe9c28, MyOpenFile, "r", false, 0 },
            { nullptr, 0xE05E0, MyReadInst, "rf", true, 0x10 },
            { nullptr, 0xC8688, MyMbtowc, "r", false, 0 },
            { nullptr, 0xE033B, MySelString, "r", false, 0 },
            { nullptr, 0x3D000, MyChangeFont, "r", false, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook 失败！", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectW", MyCFI, "1", false, 0 },
            { "user32.dll", "CreateWindowExW", MyCW, "\x03", false, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}