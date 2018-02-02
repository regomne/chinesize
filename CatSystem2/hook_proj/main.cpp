#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0x24e233,"\x9f","\xfe")
    DP(nullptr, 0x24e246,"\x7e","\xfe")
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
    constexpr auto ori_name = "\x83\x6A\x83\x85\x81\x5B\x83\x67\x83\x93\x82\xC6\x97\xD1\x8C\xE7\x82\xCC\x8E\xF7";
    if (memcmp(*str, ori_name, strlen(ori_name)) == 0) {
        *str = "『牛顿与苹果树』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));

        //static const HookPointStruct points[] = {
        //    { nullptr, 0x2F60, my_read_pic, "rf", STUB_JMP_EAX_AFTER_RETURN, 0 },
        //};

        //if (!HookFunctions(points))
        //{
        //    MessageBox(0, L"Hook 失败！", 0, 0);
        //    return TRUE;
        //}

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectA", MyCFI, "1", false, 0 },
            //{ "user32.dll", "CreateWindowExW", MyCW, "\x03", false, 0 },
            {"user32.dll","SetWindowTextA",MySWT, "\x02",false,0},
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}