#include <windows.h>
#include <D3D9.h>

#include "ilhook.h"
#include "FuncHelper.h"
#include "enc.h"

using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,sizeof(hex)-1},
PatchStruct g_Patches[] = {
    DP(nullptr, 0x226d36, "\x9f", "\x9f") //sjis char code bound
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
    //LOGERROR("charcode: %d font: %ws", lfi->lfCharSet, lfi->lfFaceName);
    if (lfi->lfCharSet == 0x86)
    {
        lfi->lfCharSet = 0x80;
        wcscpy_s(lfi->lfFaceName, L"SimHei");
    }
}

void HOOKFUNC MyCF(wchar_t** fntname)
{
    *fntname = L"SimSun";
}

void HOOKFUNC MyCW(wchar_t* class_name, wchar_t** strp)
{
    if (*strp && (((uint32_t)class_name & 0xffff0000) != 0) && wcscmp(class_name, L"D123987X") == 0)
    {
        *strp = L"『Girls Book Maker -The Beautiful Librettos-』";
    }
}

void HOOKFUNC MyGetFileInfo(const wchar_t* fpath)
{
    //auto name = string(fpath);
    LOGERROR("%ws", fpath);
}

void HOOKFUNC MyGetCharcode(Registers* regs)
{
    if (regs->eax == 932)
    {
        regs->eax = 936;
    }
}

typedef int(__cdecl* MyGetFontHandleCharCodeRoutine)(uint32_t handle);
int HOOKFUNC MyGetFontHandleCharCode(MyGetFontHandleCharCodeRoutine func, uint32_t handle)
{
    auto ret = func(handle);
    if (ret == 932)
    {
        return 936;
    }
    return ret;
}

uint16_t SjisUnicodeTable[65536];
#ifdef READ_FONT_FROM_H
#include "chars.h"
void init_fnt_table()
{
    for (size_t i = 0; i < ARRAYSIZE(UnicodeFntTable); i++)
    {
        SjisUnicodeTable[SjisFntTable[i]] = UnicodeFntTable[i];
    }
}
#else
void init_fnt_table()
{
    auto fs = fopen("font.tbl", "rb");
    if (!fs)
    {
        LOGERROR("Can't find font table!");
        return;
    }
    fread(SjisUnicodeTable, 1, 65536*2, fs);
    fclose(fs);
}
#endif


typedef int (__cdecl *MyConvStringRoutine)(const char* Src, int SrcStrLength, int SrcCharCodeFormat, char* Dest, size_t BufferBytes, int DestCharCodeFormat);
int HOOKFUNC MyConvString(MyConvStringRoutine oldfunc, const char* Src, int SrcStrLength, int SrcCharCodeFormat, char* Dest, size_t BufferBytes, int DestCharCodeFormat)
{
    if (Src != nullptr && Dest != nullptr && SrcCharCodeFormat == 936 && DestCharCodeFormat == 0x4b0)
    {
        size_t dest_i = 0;
        auto src_len = SrcStrLength < 0 ? strlen(Src) : SrcStrLength;
        for (int i = 0; i < src_len; dest_i += 2)
        {
            if (dest_i >= BufferBytes)
            {
                LOGERROR("error not enough buffer!");
                if (dest_i >= 2)
                {
                    *(wchar_t*)(Dest + dest_i - 2) = 0;
                }
                return dest_i;
            }

            auto c = (uint8_t)Src[i];
            if (c >= 0x81 && c <= 0x9f || c >= 0xe0 && c <= 0xfc)
            {
                auto real_c = *(uint16_t*)(Src + i);
                if (real_c == 0)
                {
                    LOGERROR("error char: %x %x", c, Src[i + 1]);
                }
                *(wchar_t*)(Dest + dest_i) = SjisUnicodeTable[real_c];
                i += 2;
            }
            else if (c >= 0xa0 && c < 0xe0)
            {
               *(wchar_t*)(Dest + dest_i) = (wchar_t)(c + 0xfec0);
               i++;
            }
            else
            {
               *(wchar_t*)(Dest + dest_i) = (wchar_t)c;
               i++;
            }
        }
        if (dest_i < BufferBytes)
        {
            *(wchar_t*)(Dest + dest_i) = 0;
            dest_i += 2;
        }
        return dest_i;
    }
    return oldfunc(Src, SrcStrLength, SrcCharCodeFormat, Dest, BufferBytes, DestCharCodeFormat);
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        init_fnt_table();
        //PatchMemory(g_Patches, ARRAYSIZE(g_Patches));
        //auto rva = get_ep_rva_from_module_address(GetModuleHandle(nullptr));
        static const HookPointStruct points[] = {
            //{ nullptr, rva, before_start, "r", 0, 0 },
            //{ nullptr, 0x2e33dc, MyGetCharcode, "r", 0, 0 },
            //{ nullptr, 0x2f08fd, MyGetCharcode, "r", 0, 0 },
            //{ nullptr, 0x2f09d6, MyGetCharcode, "r", 0, 0 },
            { nullptr, 0x2f0dd0, MyGetFontHandleCharCode, "f1", STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX, 0 },
            { nullptr, 0x3016e0, MyConvString, "f123456", STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX, 0 },
        };

        if (!HookFunctions(points))
        {
            MessageBox(0, L"Hook 失败！", 0, 0);
            return TRUE;
        }

        static const HookPointStructWithName points2[] = {
            //{ "gdi32.dll", "CreateFontW", MyCF, "\x0e", 0, 0 },
            { "user32.dll", "CreateWindowExW", MyCW, "2\x03", 0, 0 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}