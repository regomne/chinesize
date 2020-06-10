#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "enc.h"
#include "exhibit.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    //DP("resident.dll", 0x21F5EB,"\x80","\x86") //cp
    DP("resident.dll", 0x363360, "\x81\x74","\xa1\xb7") //右括号
    DP("resident.dll", 0x363374,"\x81\x73","\xa1\xb6") //左括号
    DP("resident.dll", 0x6a556,"\x74\x81","\xb7\xa1") //右括号，引用上面的括号所在的函数
    //DP("resident.dll", 0x540DC0, "\x81\x73","\xa1\xb6") //右括号
    //DP("resident.dll", 0x540DD0, "\x81\x74","\xa1\xb7") //左括号
    //DP("resident.dll", 0x540D30,"\x73\x81","\xb6\xa1") //左括号
    //DP("resident.dll", 0x540D40,"\x74\x81","\xb7\xa1") //右括号
    //DP("resident.dll", 0x2533F7,"\x40\x81","\xa1\xa1") //空格，call GetGlyphOutlineA之前的某个cmp，可以调试来确定
    //DP("resident.dll", 0x253C17,"\x40\x81","\xa1\xa1") //空格，call GetGlyphOutlineA之前的某个cmp，可以调试来确定
    //DP("resident.dll", 0x1E4A05,"\xa8\x03","\xa4\x03") //_mbccpy_s所在的函数内
    //DP("resident.dll", 0x540CCC,"\xa4\x03","\xa8\x03")
};
#undef DP

__declspec(naked) void D3dCreate9()
{
    __asm jmp Direct3DCreate9
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

void HOOKFUNC MyCA(char** strp)
{
    if (*strp && strcmp(*strp, "\x94\xA0\x92\xEB\x83\x8D\x83\x57\x83\x62\x83\x4E\x81\x60\x8C\xE8\x8E\x71\x82\xC6\x82\xA2\x82\xBF\x82\xE1\x82\xE7\x82\xD4\x82\x72\x82\x6C\x90\xB6\x8A\x88\x81\x60") == 0)
    {
        *strp = "箱庭逻辑 -瑚子FD-";
    }
}

void HOOKFUNC MySetMbcp(uint32_t* cp) {
    *cp = 936;
}

void HOOKFUNC MyOverwriteCP(Registers* reg) {
    reg->eax = 936;
}

extern uint32_t g_proc_cmd_return_offset;
BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));
        auto rva = get_ep_rva_from_module_address(GetModuleHandle(nullptr));

        g_proc_cmd_return_offset = 0x11C981; //是MyProcCmds函数在liteLoad中的第一个引用的返回地址
        static const HookPointStruct points[] = {
            //?liteLoad@RetouchSystem@@AAE_NPBDK@Z 函数头
            { "resident.dll", 0x11C540, MyLoadRld, "fc12", STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX, 8 },

            /*
            ?liteLoad@RetouchSystem@@AAE_NPBDK@Z 中：
            100F7375 57                                      push    edi
            100F7376 03 CB                                   add     ecx, ebx
            100F7378 89 44 24 2C                             mov     [esp+0BB0h+var_B84], eax
            100F737C E8 1F 38 FB FF                          call    ProcCmd
            100F7381 83 C7 01                                add     edi, 1
            100F7384 81 C3 D8 00 00 00                       add     ebx, 0D8h
            100F738A 3B 7C 24 18                             cmp     edi, [esp+0BA4h+var_B8C]
            100F738E 7C D0                                   jl      short loc_100F7360
            的ProcCmd的函数头
            */
            { "resident.dll", 0xDA0D0, MyProcCmds, "rfc123", STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX, 12 },

            /*ProcCmd函数内后面一个call的函数内，
            100ADC1D 51                                      push    ecx             ; lpString
            100ADC1E 68 00 01 00 00                          push    100h            ; int
            100ADC23 8B CB                                   mov     ecx, ebx
            100ADC25 E8 C6 48 11 00                          call    sub_101C24F0
            100ADC2A 8B 16                                   mov     edx, [esi]
            100ADC2C 80 3A 00                                cmp     byte ptr [edx], 0
            上面这段代码的开头
            */
            { "resident.dll", 0xD790D, MyAddString, "r", 0, 0 },

            /*
            ?loadRld@RetouchSystem@@QAEPAEPBDK@Z 函数内（graph的右下角）调用进去的一个函数内有：
            102A7A50 81 FA F0 3F 00 00                       cmp     edx, 3FF0h
            102A7A56 72 05                                   jb      short loc_102A7A5D
            102A7A58 BA F0 3F 00 00                          mov     edx, 3FF0h
            此函数函数头hook
            */
            //{ "resident.dll", 0x28EE30, MyDecRld, "fc1234", true, 16 },

            // 搜索0x3A4，找到对该值的写入位置的引用，改为0x3A8
            //{ "resident.dll", 0xDBC0D, MyOverwriteCP, "r", false, 0 },

             //{ "resident.dll", 0x1CB150, MyReadGyu, "rfc123", true, 12 },
             //{ "resident.dll", 0x1CC820, MyReadGyu2, "rfc123", true, 12 },
             //{ "resident.dll", 0x28F700, MyDecodeGyu, "rfc1", true, 4 },
             //{ "resident.dll", 0x28FB50, MyDecodeGyuAlpha, "fc", true, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook 失败！", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectA", MyCFI, "1", 0, 0 },
            { "user32.dll", "CreateWindowExA", MyCA, "\x03", false, 0 },
            { "msvcr80.dll", "_setmbcp", MySetMbcp, "\x01", 0, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}