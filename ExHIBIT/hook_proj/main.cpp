#include <windows.h>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <map>
#include <D3D9.h>

#include "ilhook.h"
#include "FileReader.h"
#include "FuncHelper.h"
#include "PicSupport.h"
#include "MyPack.h"

MyPack* g_MyPack;
using namespace std;

#define DP(name,addr,pat,hex) {name,addr,pat,hex,strlen(hex)},
struct PatchStruct
{
    char* mod_name;
    uint32_t offset;
    char* pattern;
    char* hex;
    DWORD len;
} g_Patches[] = {
    DP("resident.dll", 0x21F5EB,"\x80","\x86") //cp
    DP("resident.dll", 0x37632C,"\x81\x74","\xa1\xb7") //括号
    DP("resident.dll", 0x376340,"\x81\x73","\xa1\xb6") //括号
    DP("resident.dll", 0x21E3D7,"\x40\x81","\xa1\xa1") //空格
};
#undef DP

struct CharToReplay
{
    wchar_t uc;
    wchar_t repc;
    uint8_t ac[3];
};

vector<CharToReplay> g_rep_chars = {
    { u'♪', u'┋', "\xa9\xaf" }
};

void replace_chars(wchar_t* text, uint32_t txtlen, const vector<CharToReplay>& reps)
{
    for (uint32_t i = 0;i < txtlen;i++)
    {
        auto c = text[i];
        for (auto& rep : reps)
        {
            if (c == rep.uc)
            {
                text[i] = rep.repc;
                break;
            }
        }
    }
}

struct HookPointStruct
{
    char* module_name;
    uint32_t offset;
    void* hook_routine;
    char* reg_tags;
    bool override_eax;
    uint32_t ret_value;
};
struct HookPointStructWithName
{
    char* module_name;
    char* proc_name;
    void* hook_routine;
    char* reg_tags;
    bool override_eax;
    uint32_t ret_value;
};

bool PatchMemory()
{
    for (int i = 0;i < ARRAYSIZE(g_Patches);i++)
    {
        PatchStruct& st = g_Patches[i];
        auto mod = GetModuleHandleA(st.mod_name);
        if (!mod)
        {
            mod = LoadLibraryA(st.mod_name);
            if (!mod)
            {
                LOGERROR("Patch: Can't find module: %s", st.mod_name);
                return false;
            }
        }
        auto patch_addr = (uint8_t*)mod + st.offset;

        if (st.pattern != nullptr)
        {
            if (memcmp(patch_addr, st.pattern, st.len) != 0)
            {
                return false;
            }
        }
        DWORD oldProt;
        if (!VirtualProtect(patch_addr, st.len, PAGE_EXECUTE_READWRITE, &oldProt))
        {
            return false;
        }
        memcpy(patch_addr, st.hex, st.len);
    }
    return true;
}

bool HookFunctions(const HookPointStruct* hooks, uint32_t cnt)
{
    auto buff = (uint8_t*)VirtualAlloc(0, cnt * 100, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!buff)
    {
        LOGERROR("Hook: no memory!");
        return false;
    }

    HookSrcObject src;
    HookStubObject stub;

    for (size_t i = 0;i < cnt;i++)
    {
        auto hook = &hooks[i];
        auto mod = GetModuleHandleA(hook->module_name);
        if (!mod)
        {
            mod = LoadLibraryA(hook->module_name);
            if (!mod)
            {
                LOGERROR("Hook: Can't find module: %s, error: %d", hook->module_name, GetLastError());
                return false;
            }
        }
        auto addr = (uint8_t*)mod + hook->offset;
        uint32_t hook_flag = hook->override_eax ? (STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX) : 0;
        if (!InitializeHookSrcObject(&src, addr, true) ||
            !InitializeStubObject(&stub, buff + i * 100, 100, hook->ret_value, hook_flag) ||
            !Hook32(&src, 0, &stub, hook->hook_routine, hook->reg_tags))
        {
            LOGERROR("Hook: Can't hook module: %s, offset: 0x%x", hook->module_name, hook->offset);
            return false;
        }
    }
    return true;
}

bool HookFunctions(const HookPointStructWithName* hooks, uint32_t cnt)
{
    auto buff = (uint8_t*)VirtualAlloc(0, cnt * 100, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!buff)
    {
        LOGERROR("Hook: no memory!");
        return false;
    }

    HookSrcObject src;
    HookStubObject stub;

    for (size_t i = 0;i < cnt;i++)
    {
        auto hook = &hooks[i];
        auto mod = GetModuleHandleA(hook->module_name);
        if (!mod)
        {
            mod = LoadLibraryA(hook->module_name);
            if (!mod)
            {
                LOGERROR("Hook: Can't find module: %s", hook->module_name);
                return false;
            }
        }
        auto addr = GetProcAddress(mod, hook->proc_name);
        if (!addr)
        {
            LOGERROR("Hook: Can't find %s in module: %s", hook->proc_name, hook->module_name);
            return false;
        }
        uint32_t hook_flag = hook->override_eax ? (STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX) : 0;
        if (!InitializeHookSrcObject(&src, addr, true) ||
            !InitializeStubObject(&stub, buff + i * 100, 100, hook->ret_value, hook_flag) ||
            !Hook32(&src, 0, &stub, hook->hook_routine, hook->reg_tags))
        {
            LOGERROR("Hook: Can't hook module: %s, name: %s", hook->module_name, hook->proc_name);
            return false;
        }
    }
    return true;
}

template <size_t _Size>
bool HookFunctions(const HookPointStruct(&strt)[_Size])
{
    return HookFunctions(strt, _Size);
}
template <size_t _Size>
bool HookFunctions(const HookPointStructWithName(&strt)[_Size])
{
    return HookFunctions(strt, _Size);
}

__declspec(naked) void D3dCreate9()
{
    __asm jmp Direct3DCreate9
}

std::string ReplaceAll(const std::string& str_ori, const std::string& from, const std::string& to) 
{
    size_t start_pos = 0;
    std::string str(str_ori);
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return std::move(str);
}
vector<string>* SplitTxt(NakedMemory& txt)
{
    auto buff = (uint8_t*)txt.Get();
    uint32_t i = 0;
    uint32_t last = 0;
    vector<string>* ls = new vector<string>();
    while (true)
    {
        while (i < txt.GetSize() && buff[i] != '\r') i++;
        ls->push_back(ReplaceAll(string((char*)buff + last, i - last), "\\n", "\n"));
        if (i + 1 >= txt.GetSize()) break;
        i += 2;
        last = i;
    }
    return ls;
}

NakedMemory CvtToAnsi(NakedMemory& str, int cp)
{
    auto buff = (uint8_t*)str.Get();
    if (*(WORD*)buff == 0xfeff)
    {
        auto newsize = WideCharToMultiByte(cp, 0, (wchar_t*)(buff + 2), (str.GetSize() - 2) / 2, 0, 0, 0, 0);
        NakedMemory ansi(newsize);
        WideCharToMultiByte(cp, 0, (wchar_t*)(buff + 2), (str.GetSize() - 2) / 2, (char*)ansi.Get(), ansi.GetSize(), 0, 0);
        return std::move(ansi);
    }
    else if (str.GetSize() > 3 && buff[0] == 0xef && buff[1] == 0xbb && buff[2] == 0xbf)
    {
        if (cp == CP_UTF8)
        {
            NakedMemory ansi(str.GetSize() - 3);
            memcpy(ansi.Get(), buff + 3, ansi.GetSize());
            return std::move(ansi);
        }
        else
        {
            NakedMemory u16(str.GetSize() * 2);
            auto newsize1 = MultiByteToWideChar(CP_UTF8, 0, (char*)buff + 3, str.GetSize() - 3, (wchar_t*)u16.Get(), str.GetSize());

            auto newsize = WideCharToMultiByte(cp, 0, (wchar_t*)u16.Get(), newsize1, 0, 0, 0, 0);
            NakedMemory ansi(newsize);
            WideCharToMultiByte(cp, 0, (wchar_t*)u16.Get(), newsize1, (char*)ansi.Get(), ansi.GetSize(), 0, 0);
            return std::move(ansi);
        }
    }
    else
    {
        return str;
    }
}

vector<string>* g_lines = nullptr;
string g_cur_txt;
uint32_t g_line_idx = 0;
uint32_t HOOKFUNC MyLoadRld(void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2)
{
    if (!g_MyPack) {
        g_MyPack = new MyPack(L"cnpack");
        if (!g_MyPack->Init()) {
            //todo
        }
    }
    auto fname = string((char*)arg1);
    if (fname.find("rld\\") != 0)
    {
        return ThiscallFunction2(old_proc, thisp, arg1, arg2);
    }
    auto pos = fname.find(".rld");
    auto txtName = string("txt\\") + fname.substr(4, pos - 4) + ".txt";
    g_cur_txt = txtName;
    auto mem = g_MyPack->ReadAFile(txtName);
    if (!mem.Get()) {
        //Log("Can't read %s", txtName.c_str());
        return ThiscallFunction2(old_proc, thisp, arg1, arg2);
    }
    auto ansi = CvtToAnsi(mem, 936);
    g_lines = SplitTxt(ansi);
    g_line_idx = 0;

    auto ret = ThiscallFunction2(old_proc, thisp, arg1, arg2);

    delete g_lines;
    g_lines = nullptr;
    g_line_idx = 0;
    g_cur_txt = "";
    return ret;
}

bool g_enter_proc = false;
uint32_t g_cur_cmd = -1;

uint32_t HOOKFUNC MyProcCmds(Registers* regs, void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    static auto mod_addr = (intptr_t)GetModuleHandleA("resident.dll");
    if (*(uint32_t*)regs->esp == mod_addr + 0xF7381)
    {
        g_enter_proc = true;
        g_cur_cmd = arg2;
    }
    auto ret = ThiscallFunction3(old_proc, thisp, arg1, arg2, arg3);
    g_enter_proc = false;
    g_cur_cmd = -1;
    return ret;
}

char* ReadOneLineFromTxt()
{
    static char line[1000];
    if (g_lines)
    {
        while (g_line_idx < g_lines->size())
        {
            auto& ls = (*g_lines)[g_line_idx];
            if (ls.length() > 0 && ls[0] != '$')
            {
                if (ls.length() > sizeof(line))
                {
                    LOGERROR("%s line %d too long", g_cur_txt.c_str(), g_line_idx + 1);
                    return nullptr;
                }
                ls.copy(line, ls.length());
                line[ls.length()] = '\0';
                g_line_idx++;
                return line;
            }
            g_line_idx++;
        }
    }
    return nullptr;
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

void HOOKFUNC MyAddString(Registers* regs)
{
    if (g_enter_proc && g_lines)
    {
        auto old_str = string((const char*)regs->ecx);
        auto str_idx = regs->edi;
        auto cmd = g_cur_cmd & 0xffff;
        if (cmd == 28)
        {
            if (old_str.length() != 0 &&
                old_str != "*" &&
                old_str != "$noname$" &&
                std::count(old_str.begin(), old_str.end(), ',') < 2)
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }
        }
        else if (cmd == 21)
        {
            if (old_str.length() != 0 &&
                old_str != "*" &&
                old_str != "$noname$" &&
                std::count(old_str.begin(), old_str.end(), ',') < 2)
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }
        }
        else if (cmd == 48)
        {
            if (str_idx == 0)
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }

        }
        else if (cmd == 191)
        {
            if (str_idx == 0 && !IsHalf(old_str))
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }
        }
    }
}

#include "rld_key.cpp"
void HOOKFUNC MyDecRld(void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    if (arg4 == 0x0AE85A916)
    {
        ThiscallFunction4(old_proc, thisp, arg1, arg2, arg3, arg4);
    }
    else
    {
        auto dec_len = arg3;
        if (dec_len > 0x3ff0 * 4)
        {
            dec_len = 0x3ff0 * 4;
        }
        else
        {
            dec_len &= 0xfffffffc;
        }
        auto buff = (uint8_t*)arg2;
        for (size_t i = 0;i < dec_len;i++)
        {
            buff[i] ^= g_rld_key[i & 0x3ff];
        }
    }
}

void HOOKFUNC MyCFI(LPLOGFONTA lfi)
{
    //if (lfi->lfCharSet == 0x80)
    //{
    //    lfi->lfCharSet = 0x86;
    //}
    if (strcmp(lfi->lfFaceName, "\x82\x6C\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4E") == 0)
    {
        strcpy_s(lfi->lfFaceName, "Microsoft Yahei");
    }
}
typedef DWORD (WINAPI *GetGlyphOutlineARoutine)(
    _In_        HDC            hdc,
    _In_        UINT           uChar,
    _In_        UINT           uFormat,
    _Out_       LPGLYPHMETRICS lpgm,
    _In_        DWORD          cbBuffer,
    _Out_       LPVOID         lpvBuffer,
    _In_  const MAT2           *lpmat2
);

HFONT create_font()
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = -24;
    lf.lfWeight = 400;
    lf.lfQuality = 3;
    lf.lfPitchAndFamily = 1;
    wcscpy_s(lf.lfFaceName, L"SimSun");
    auto hf = CreateFontIndirect(&lf);
    return hf;
}

class GlyphGen
{
public:
    GlyphGen(HDC hdc)
    {
        song_dc_ = CreateCompatibleDC(hdc);
        song_font_ = create_song_font();
        SelectObject(song_dc_, song_font_);
        basic_dc_ = CreateCompatibleDC(hdc);
        basic_font_ = create_basic_font();
        SelectObject(basic_dc_, basic_font_);
    }

    ~GlyphGen()
    {
        DeleteDC(song_dc_);
        DeleteObject(song_font_);
        DeleteDC(basic_dc_);
        DeleteObject(basic_font_);
    }

    DWORD get_song_glyph(
        _In_        UINT           uChar,
        _In_        UINT           uFormat,
        _Out_       LPGLYPHMETRICS lpgm,
        _In_        DWORD          cbBuffer,
        _Out_       LPVOID         lpvBuffer,
        _In_  const MAT2           *lpmat2)
    {
        return GetGlyphOutlineW(song_dc_, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
    }

    DWORD get_basic_glyph(
        _In_        UINT           uChar,
        _In_        UINT           uFormat,
        _Out_       LPGLYPHMETRICS lpgm,
        _In_        DWORD          cbBuffer,
        _Out_       LPVOID         lpvBuffer,
        _In_  const MAT2           *lpmat2)
    {
        return GetGlyphOutlineW(basic_dc_, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
    }

private:
    HFONT create_song_font()
    {
        LOGFONT lf;
        memset(&lf, 0, sizeof(lf));
        lf.lfHeight = -24;
        lf.lfWeight = 400;
        lf.lfQuality = 3;
        lf.lfPitchAndFamily = 1;
        wcscpy_s(lf.lfFaceName, L"SimSun");
        auto hf = CreateFontIndirect(&lf);
        return hf;
    }

    HFONT create_basic_font()
    {
        LOGFONT lf;
        memset(&lf, 0, sizeof(lf));
        lf.lfHeight = -24;
        lf.lfWeight = 0x190;
        lf.lfQuality = 2;
        lf.lfPitchAndFamily = 1;
        wcscpy_s(lf.lfFaceName, L"SimHei");
        auto hf = CreateFontIndirect(&lf);
        return hf;
    }
private:
    HDC song_dc_;
    HDC basic_dc_;
    HFONT song_font_;
    HFONT basic_font_;
};

DWORD HOOKFUNC MyGGOA(
    GetGlyphOutlineARoutine old_func,
    _In_        HDC            hdc,
    _In_        UINT           uChar,
    _In_        UINT           uFormat,
    _Out_       LPGLYPHMETRICS lpgm,
    _In_        DWORD          cbBuffer,
    _Out_       LPVOID         lpvBuffer,
    _In_  const MAT2           *lpmat2
    )
{
    static auto font_gen = new GlyphGen(hdc);
    for (auto& rep : g_rep_chars)
    {
        if (rep.ac[1] == (uChar & 0xff) && rep.ac[0] == (uChar >> 8))
        {
            return font_gen->get_song_glyph(rep.uc, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
        }
    }
    char ach[2];
    wchar_t wch[2];
    if (uChar >= 0x80) {
        ach[0] = uChar >> 8;
        ach[1] = uChar & 0xff;
    }
    else {
        ach[0] = uChar;
        ach[1] = 0;
    }
    MultiByteToWideChar(936, 0, ach, 2, wch, 2);
    return GetGlyphOutlineW(hdc, wch[0], uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
    return font_gen->get_basic_glyph(wch[0], uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
    //return old_func(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
}
typedef void* (__cdecl *exhibit_alloc_routine)(uint32_t size, uint32_t flag);
typedef void (__cdecl *exhibit_free_routine)(void* size, uint32_t flag);
exhibit_alloc_routine g_exhibit_alloc = nullptr;
//exhibit_free_routine g_exhibit_free = nullptr;

NakedMemory* g_png_rgb;
NakedMemory* g_png_alpha;
struct PngInfos
{
    int width;
    int height;
    int bit_count;
} g_png_info;

void* gen_exhibit_bmp(int w, int h, int bit, NakedMemory& mem, NakedMemory* pal)
{
    auto pal_size = 0;
    if (bit == 8)
    {
        pal_size = 0x400;
    }
    auto ptr = (uint8_t*)g_exhibit_alloc(mem.GetSize()+ pal_size + sizeof(BITMAPINFOHEADER),
        HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS);
    auto info = (BITMAPINFOHEADER*)ptr;
    info->biSize = sizeof(*info);
    info->biWidth = w;
    info->biHeight = h;
    info->biBitCount = bit;
    info->biPlanes = 1;
    info->biCompression = 0;
    info->biSizeImage = mem.GetSize();
    info->biXPelsPerMeter = 0;
    info->biYPelsPerMeter = 0;
    info->biClrUsed = bit == 8 ? 0x100 : 0;
    info->biClrImportant = 0;
    
    auto next_info = (uint8_t*)(info + 1);
    if (bit == 8)
    {
        if (!pal)
        {
            auto p = next_info;
            for (int i = 0;i < 0x100;i++)
            {
                p[0] = i;
                p[1] = i;
                p[2] = i;
                p[3] = 0;
                p += 4;
            }
            next_info = p;
        }
        else
        {
            memcpy(next_info, pal->Get(), 0x400);
            next_info += 0x400;
        }
    }

    memcpy(next_info, mem.Get(), mem.GetSize());
    return ptr;
}

bool read_png_to_dib(const string& png_name, NakedMemory** rgb, NakedMemory** alpha, PngInfos* info)
{
    MyFileReader reader;
    auto mem = reader.ReadToMem(png_name.c_str());
    if (!mem.Get())
    {
        LOGERROR("%s read fail.", png_name.c_str());
        return false;
    }
    NakedMemory dib;
    int width, height, bit_count;
    if (!ReadPngToBmp(mem, &width, &height, &bit_count, dib))
    {
        LOGERROR("%s parse fail.", png_name.c_str());
        return false;
    }

    auto round_4 = [](uint32_t val) {
        return (val + 3) & 0xfffffffc;
    };

    auto dib_size = height * round_4(width * (bit_count / 8));
    if (dib_size != dib.GetSize())
    {
        LOGERROR("%s parse format error.", png_name.c_str());
        return nullptr;
    }

    if (bit_count == 24)
    {
        *rgb = new NakedMemory(std::move(dib));
        *alpha = nullptr;
    }
    else if (bit_count == 32)
    {
        *alpha = new NakedMemory();
        *rgb = new NakedMemory(Dib32To24(width, height, dib, **alpha));
    }
    info->width = width;
    info->height = height;
    info->bit_count = bit_count;
    return true;
}

uint32_t HOOKFUNC MyReadGyu(Registers* regs, void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
//     static auto mod_addr = (intptr_t)GetModuleHandleA("resident.dll");
//     if(regs->esi)
    string gyu_name((char*)arg1);
    auto pos = gyu_name.find(".gyu");
    auto png_name = string("png\\") + gyu_name.substr(4, pos - 4) + ".png";
    if (exists_file(png_name.c_str()))
    {
        read_png_to_dib(png_name, &g_png_rgb, &g_png_alpha, &g_png_info);
    }
    auto ret = ThiscallFunction3(old_proc, thisp, arg1, arg2, arg3);
    
    delete g_png_alpha;
    delete g_png_rgb;
    g_png_alpha = nullptr;
    g_png_rgb = nullptr;

    return ret;
}

uint32_t HOOKFUNC MyReadGyu2(Registers* regs, void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    //     static auto mod_addr = (intptr_t)GetModuleHandleA("resident.dll");
    //     if(regs->esi)
    string gyu_name((char*)arg1);
    auto pos = gyu_name.find(".gyu");
    auto png_name = string("png\\") + gyu_name.substr(4, pos - 4) + ".png";
    if (exists_file(png_name.c_str()))
    {
        read_png_to_dib(png_name, &g_png_rgb, &g_png_alpha, &g_png_info);
    }
    auto ret = ThiscallFunction3(old_proc, thisp, arg1, arg2, arg3);

    delete g_png_alpha;
    delete g_png_rgb;
    g_png_alpha = nullptr;
    g_png_rgb = nullptr;

    return ret;
}

void* HOOKFUNC MyDecodeGyu(Registers* regs, void* old_proc, void* thisp, uint32_t arg1)
{
    if (g_png_rgb)
    {
        auto gyu_bitcnt = *(uint32_t*)regs->ecx + 12;
        if (gyu_bitcnt == 24)
        {
            auto ptr = gen_exhibit_bmp(g_png_info.width, g_png_info.height, 24, *g_png_rgb, nullptr);
            if (ptr)
            {
                return ptr;
            }
        }
        else
        {
            NakedMemory pal;
            NakedMemory nrgb;
            void* ptr = nullptr;
            if (decolor24(g_png_info.width, g_png_info.height, *g_png_rgb, pal, nrgb) &&
                (ptr = gen_exhibit_bmp(g_png_info.width, g_png_info.height, 8, nrgb, &pal)))
            {
                return ptr;
            }
        }
    }
    return (void*)ThiscallFunction1(old_proc, thisp, arg1);
}

void* HOOKFUNC MyDecodeGyuAlpha(void* old_proc, void* thisp)
{
    if (g_png_alpha)
    {
        auto ptr = gen_exhibit_bmp(g_png_info.width, g_png_info.height, 8, *g_png_alpha, nullptr);
        if (ptr)
        {
            return ptr;
        }
    }
    return (void*)ThiscallFunction0(old_proc, thisp);
}

void HOOKFUNC MyCWE(const char* className, char** windowName) {
    if (strcmp(className, "lovesis.wndclass") == 0 && *windowName) {
        *windowName = "『Love Love Sisters』中文版 | 黙示游戏中文化兴趣小组 译制 | 交流群号：153454926";
    }
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        PatchMemory();

        auto mod = (uint8_t*)LoadLibrary(L"resident.dll");
        g_exhibit_alloc = (exhibit_alloc_routine)(mod + 0x2245C0);
        //g_exhibit_free = (exhibit_free_routine)(mod + 0x2234f0);

        static const HookPointStruct points[] = {
            { "resident.dll", 0xf6f40, MyLoadRld, "fc12", true, 8 },
            { "resident.dll", 0xAABA0, MyProcCmds, "rfc123", true, 12 },
            { "resident.dll", 0xA80CD, MyAddString, "r", false, 0 },
            { "resident.dll", 0x28EE30, MyDecRld, "fc1234", true, 16 },

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
            { "gdi32.dll", "CreateFontIndirectA", MyCFI, "1", false, 0 },
            { "user32.dll", "CreateWindowExA", MyCWE, "2\x03", false, 0 },
            { "gdi32.dll", "GetGlyphOutlineA", MyGGOA, "f1234567", true, 28 },
        };
        if (!HookFunctions(points2))
        {
            MessageBox(0, L"Hook2 失败！", 0, 0);
        }
    }
    return TRUE;
}