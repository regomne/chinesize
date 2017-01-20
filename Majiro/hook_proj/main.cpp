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

#include "mjo.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0x44ddd,"\x75\x81","\xb8\xa1") //quote
    DP(nullptr, 0x44df5,"\x76\x81","\xb9\xa1")
    DP(nullptr, 0x44e31,"\x75\x81","\xb8\xa1")
    DP(nullptr, 0x45041,nullptr,"\xa1\xa1")
    DP(nullptr, 0x4523d,nullptr,"\xb8\xa1")
    DP(nullptr, 0x452ad,nullptr,"\xb9\xa1")
    DP(nullptr, 0x424b3,nullptr,"\xa1")
    DP(nullptr, 0x424b9,nullptr,"\xa1")
    DP(nullptr, 0x424c6,nullptr,"\xa1\xa1")
    DP(nullptr, 0x42592,nullptr,"\xa1")
    DP(nullptr, 0x42598,nullptr,"\xa1")
    DP(nullptr, 0x425a5,nullptr,"\xa1\xa1")
    DP(nullptr, 0x42535,nullptr,"\xa1")
    DP(nullptr, 0x4253b,nullptr,"\xa1")
    DP(nullptr, 0x42610,nullptr,"\xa1")
    DP(nullptr, 0x42616,nullptr,"\xa1")

    DP(nullptr, 0x44478,nullptr,"\xeb")
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
    if (lfi->lfCharSet == 0x80)
    {
        lfi->lfCharSet = 0x86;
    }
    //"£Í£Ó ¥´¥·¥Ã¥¯"
    if (strcmp(lfi->lfFaceName, "\x82\x6C\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4E") == 0)
    {
        strcpy_s(lfi->lfFaceName, "YouYuan");
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
        PatchMemory(g_Patches, ARRAYSIZE(g_Patches));
        auto mod = (uint8_t*)GetModuleHandle(nullptr);
        auto table = mod + 0xaf3c9;
        DWORD old_prot;
        VirtualProtect(table, 0x100, PAGE_READWRITE, &old_prot);
        for (int i = 0x80;i <= 0xfe;i++)
        {
            table[i] |= 1;
        }
        VirtualProtect(mod + 0xaad00, 0x300, PAGE_READWRITE, &old_prot);
        uint32_t offsets[] = {
            0xaad00,0xaad08,0xaad10,0xaad18,0xaad90,0xaae04,0xaae2c,0xaae30,0xaae34,0xaae38,0xaae3c,0xaae40,0xaaf88,0xaaf90
        };
        wchar_t temp[200];
        for (int i = 0;i < ARRAYSIZE(offsets);i++)
        {
            auto addr = (char*)mod + offsets[i];
            MultiByteToWideChar(932, 0, addr, -1, temp, 200);
            WideCharToMultiByte(936, 0, temp, -1, addr, 200, 0, 0);
        }

        static const HookPointStruct points[] = {
            { nullptr, 0x895e6, MyFopen, "f12", true, 0 },
            { nullptr, 0x8936c, MyFclose, "f1", true, 0 },
            { nullptr, 0x8bc69, MyFread, "f1234", true, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook Ê§°Ü£¡", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            { "gdi32.dll", "CreateFontIndirectA", MyCFI, "1", false, 0 },
            { "user32.dll", "CreateWindowExA", MyCW, "\x03", false, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 Ê§°Ü£¡", 0, 0);
        }
    }
    return TRUE;
}