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
    if (*strp && wcscmp(*strp, L"恋する気持ちのかさねかた") == 0)
    {
        *strp = L"『恋する気持ちのかさねかた』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        InitWs2();

        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));

        static const HookPointStruct points[] = {
            { nullptr, 0xd2820, MyOpenFile1, "12", false, 0 },
            { nullptr, 0xd2d60, MyOpenFile, "r", false, 0 },

            //0F B6 ?? ?? ?? 89 5C 24 ?? 8B 2C 85 ?? ?? ?? 00 85 ED 0F 84
            //找到一个函数，此函数内有三次对ReadInst函数的call
            { nullptr, 0xC99B0, MyReadInst, "rf", true, 0x10 },
            
            //8B 45 ?? 2B 45 ?? 8B 33 C7 44 24 ?? 01 00 00 00 3B F0 72 05 E8
            //找到一个函数，下面有个参数是lpMultiByteStr的call
            { nullptr, 0xC970B, MySelString, "r", false, 0 },
            //此函数在上面那个call之内
            { nullptr, 0xB1708, MyMbtowc, "r", false, 0 },
            //wcsncmp的第二个引用所在的函数
            { nullptr, 0x2E160, MyChangeFont, "r", false, 0 },
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