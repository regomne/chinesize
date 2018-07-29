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
    if (*strp && wcscmp(*strp, L"桜舞う乙女のロンド～お兄ちゃんじゃなくて恋人になって欲しいの～") == 0)
    {
		*strp = L"『　　　　　　　　　　　　　　　　　　　　　　　　　　』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        InitWs2();

        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));
		
        static const HookPointStruct points[] = {
            //_wfopen_s的所在函数，一个是函数头，一个函数中间call _wfopen_s用来读取arc的位置
            { nullptr, 0xC8A20, MyOpenFile1, "12", false, 0 }, //参数位置也需要适配
            { nullptr, 0xC8F60, MyOpenFile, "r", false, 0 },

            //1.5版本 0F B6 ?? ?? ?? 89 5C 24 ?? 8B 2C 85 ?? ?? ?? 00 85 ED 0F 84
            //根据特征码找到一个函数，其内有三次对ReadInst函数的call，hook ReadInst的函数头
            { nullptr, 0xC0640, MyReadInst_v15, "rf", true, 0x10 },

            //1.7版本 0f b6 45 ?? 33 ff 89 4d ?? 8b 04 85 ?? ?? ?? ?? 89 45 
            //根据特征码找到一个函数，其内有三次对ReadInst函数的call，hook ReadInst的函数头
			//方法2：从defaultInst中找到inst0，然后用二进制在exe中搜索，找到对该地址的引用，即为上述特征码所在的函数
            //{ nullptr, 0xD5B70, MyReadInst_v17, "rf", true, 0x14 },
            
            //1.5版本 8B 45 ?? 2B 45 ?? 8B 33 C7 44 24 ?? 01 00 00 00 3B F0 72 05 E8
            //此特征码头部即为hook地址，其下几十行之内有个参数是lpMultiByteStr的call
            { nullptr, 0xC0310, MySelString_v15, "r", false, 0 },

            //1.7版本 C7 45 FC 02 00 00 00 8d 55 ?? 8b 4e ?? 03 0f e8
            //此特征码尾部的call指令即为hook地址，此处call的函数就是MyMbtowc hook位置所在的函数
            //或者找到MyReadInst_v17的特征码所在的函数，往上回溯一到两层，找到Variant相关的函数，然后找参数是lpMultiByteStr的call
            //{ nullptr, 0xD5898, MySelString_v17, "r", false, 0 },

            //此位置在上面那个call之内，call MultiByteToWideChar的位置
            { nullptr, 0xA8078, MyMbtowc, "r", false, 0 },

            //wcsncmp的第二个引用所在的函数头
            { nullptr, 0x2BAE0, MyChangeFont_v15, "r", false, 0 },
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