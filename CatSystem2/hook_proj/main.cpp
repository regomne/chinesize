#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "enc.h"
#include "cs2.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0x177463,"\x9f","\xfe")
    DP(nullptr, 0x177476,"\x7e","\xfe")
};
#undef DP

__declspec(naked) void D3dCreate9()
{
    __asm jmp Direct3DCreate9
}

__declspec(naked) void MyOFNA() {
    __asm jmp GetOpenFileNameA
}

__declspec(naked) void MySFNA() {
    __asm jmp GetSaveFileNameA
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
    if (strcmp(lfi->lfFaceName, "\x82\x6C\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4e") == 0)
    {
        strcpy_s(lfi->lfFaceName, "SimHei");
    }
}

void HOOKFUNC MyCW(wchar_t** strp)
{
    if (*strp && wcscmp(*strp, L"想いを捧げる乙女のメロディー") == 0)
    {
        *strp = L"『有少女献鸣的爱之奏章』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

void HOOKFUNC MySWT(char** str) {
    constexpr auto ori_name = "\x97\xF6\x83\x6A\x81\x41\x8A\xC3\x96\xA1\x83\x92\x83\x5C\x83\x47\x83\x65\x81\x69\x44\x4C\x94\xC5\x81\x6A";
    if (memcmp(*str, ori_name, strlen(ori_name)) == 0) {
        *str = "『尚有佳蜜伴茶馨』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}


BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));

        auto rva = get_ep_rva_from_module_address(GetModuleHandle(nullptr));
        static const HookPointStruct points[] = {
            { nullptr, 0x236F25, before_start, "r", 0, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook 失败！", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectA", MyCFI, "1", false, 0 },
            //{ "user32.dll", "CreateWindowExW", MyCW, "\x03", false, 0 },
            {"user32.dll","SetWindowTextA",MySWT, "\x02",false,0},
            {"kernel32.dll","GetVolumeInformationA",MyGetVolumeInformation,"f12345678",true,32}
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}