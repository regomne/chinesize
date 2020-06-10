#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "bgi.h"
#include "enc.h"
#include "license.h"
#include "resource.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0x12345, "\x9f\x00", "\xfe\x00") //char bound
    /*DP(nullptr, 0x3e5eb, "\x74\x1d", "\x90\x90") //some char comparison
    DP(nullptr, 0x3e5f0, "\x74\x0e", "\x90\x90") //some char comparison
    DP(nullptr, 0x3e4df, "\x75", "\xeb") //some char comparison*/
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

void HOOKFUNC MyCFI(uint32_t* charset, wchar_t** font_name)
{
    if (*charset == 0x80)
    {
        *charset = 0x86;
    }
    *font_name = L"SimHei";
}

//void HOOKFUNC Mymbcs()

void HOOKFUNC MyCW(wchar_t** strp)
{
    if (*strp && wcscmp(*strp, L"想いを捧げる乙女のメロディー") == 0)
    {
        *strp = L"『有少女献鸣的爱之奏章』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

void HOOKFUNC MyMBW(int cp)
{
    if (cp == CP_UTF8)
    {
        LOGERROR("cp:");
    }
}

void HOOKFUNC MyWTB(int cp, wchar_t* s)
{
    if (cp == 932 && wcscmp(s, L"忠臣时代") == 0)
    {
        __asm {
            __emit 0xcc;
        }
        //LOGERROR(L"cp:%d, s: %ws", cp, s);
    }
}

void HOOKFUNC MySWT(wchar_t** title)
{
    *title = L"『你喜欢的是哪个我呢？』中文版";
}

void HOOKFUNC on_patch_wcstombs(Registers* regs)
{
    regs->esi = CP_UTF8;
}

BOOL WINAPI DllMain(_In_ void* dll_module, _In_ unsigned long reason, _In_opt_ void*)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        on_oep();
        //auto rva = get_ep_rva_from_module_address(GetModuleHandle(nullptr));
        static const HookPointStruct points[] = {
            // fxckBGI获取的解压函数地址，call该函数的xref中的一个
            // jmp的地址是下一条，意即直接跳过此次call
            // 注意bgi.cpp中有一个函数地址的硬编码，是此解压函数的地址
            { nullptr, 0x6f4fd, on_load_rsc, "r", STUB_JMP_ADDR_AFTER_RETURN, 0x6f502 },

            // 用于解决人名在显示之前将utf8重新转换为sjis的问题
            // hook地址是一次call WideCharToMultiByte的位置，其上面有cmp 0xfde9/mov 0x3a4相关的指令
            { nullptr, 0x9edb7, on_patch_wcstombs, "r", 0, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook 失败！", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontW", MyCFI, "\x09\x0e", 0, 0 },
            { "user32.dll", "SetWindowTextW", MySWT, "\x02", 0, 0 },
            //{ "kernel32.dll", "MultiByteToWideChar", MyMBW, "1", 0, 0 },
            //{ "kernel32.dll", "WideCharToMultiByte", MyWTB, "13", 0, 0 },

            //{ "user32.dll", "CreateWindowExW", MyCW, "\x03", 0, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}