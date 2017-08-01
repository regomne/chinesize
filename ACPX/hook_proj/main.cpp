#include <ilhook.h>
#include <windows.h>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <D3D9.h>

#include "FileReader.h"

using namespace std;

#define DP(addr,pat,hex) {(void*)addr,pat,hex,sizeof(hex)-1},
struct PatchStruct
{
    void* addr;
    char* pattern;
    char* hex;
    DWORD len;
} g_Patches[] = {
    DP(0x00506AF5,"\x80","\x86")
    DP(0x00450368,"\x9f","\xfe")
    DP(0x568CB0, nullptr, "退出确认\0")
    DP(0x568CA0, nullptr, "要退出游戏吗？\0")
};
#undef DP

struct CharToReplay
{
    wchar_t uc;
    wchar_t repc;
    char ac[3];
};

vector<CharToReplay> g_rep_chars = {
    { u'♪', u'┋', "\xa9\xaf" }
};

void replace_chars(wchar_t* text, uint32_t txtlen, const vector<CharToReplay>& reps)
{
    for (int i = 0;i < txtlen;i++)
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

void Log(wchar_t* format, ...)
{
    wchar_t buffer[0x1000];
    va_list ap;
    va_start(ap, format);
    auto char_cnt = vswprintf_s(buffer, format, ap);
    auto fp = fopen("log.log", "ab+");
    fwrite(buffer, 1, char_cnt * 2, fp);
    fwrite(L"\r\n", 1, 4, fp);
    fclose(fp);
    va_end(ap);
}

void Log(char* format, ...)
{
    char buffer[0x1000];
    va_list ap;
    va_start(ap, format);
    auto char_cnt = vsprintf_s(buffer, format, ap);
    auto fp = fopen("log.log", "ab+");
    fwrite(buffer, 1, char_cnt, fp);
    fwrite("\r\n", 1, 2, fp);
    fclose(fp);
    va_end(ap);
}

bool PatchMemory()
{
    for (int i = 0;i < ARRAYSIZE(g_Patches);i++)
    {
        PatchStruct& st = g_Patches[i];
        if (st.pattern != nullptr)
        {
            if (memcmp(st.addr, st.pattern, st.len) != 0)
            {
                return false;
            }
        }
        DWORD oldProt;
        if (!VirtualProtect(st.addr, st.len, PAGE_EXECUTE_READWRITE, &oldProt))
        {
            return false;
        }
        memcpy(st.addr, st.hex, st.len);
    }
    return true;
}
__declspec(naked) void D3dCreate9()
{
    __asm jmp Direct3DCreate9
}

struct ExtFileHandleInfo
{
    DWORD isSubFile;
    HANDLE file;
    DWORD filePos;
    DWORD size;
};

struct ExtFileInfo
{
    DWORD isMem;
    void* buff;
    DWORD unk;
    DWORD size;
    ExtFileHandleInfo* handleInfo;
};

void FreeFileInfo(ExtFileInfo* info)
{
    if (info->isMem)
    {
        GlobalUnlock(info->buff);
        GlobalFree(info->buff);
    }
    else
    {
        CloseHandle(info->handleInfo->file);
        GlobalUnlock(info->handleInfo);
        GlobalFree(info->handleInfo);
    }
    GlobalUnlock(info);
    GlobalFree(info);
}

ExtFileInfo* AllocFileInfo(MemMan& mem)
{
    auto info = (ExtFileInfo*)GlobalAlloc(GMEM_ZEROINIT, sizeof(ExtFileInfo));
    GlobalLock(info);
    auto buff = (uint8_t*)GlobalAlloc(0, mem.GetSize());
    if (!buff)
    {
        GlobalUnlock(info);
        GlobalFree(info);
        return nullptr;
    }
    memcpy(buff, mem.Get(), mem.GetSize());
    info->isMem = 1;
    info->buff = buff;
    info->size = mem.GetSize();
    return info;
}

vector<string> SplitTxt(MemMan& txt)
{
    auto buff = (uint8_t*)txt.Get();
    uint32_t i = 0;
    uint32_t last = 0;
    vector<string> ls;
    while (true)
    {
        while (i < txt.GetSize() && buff[i] != '\r') i++;
        ls.push_back(string((char*)buff + last, i - last));
        if (i + 1 >= txt.GetSize()) break;
        i += 2;
        last = i;
    }
    return ls;
}

MemMan CvtToAnsi(MemMan& str)
{
    auto buff = (uint8_t*)str.Get();
    if (*(WORD*)buff == 0xfeff)
    {
        auto txtlen = (str.GetSize() - 2) / 2;
        replace_chars((wchar_t*)(buff + 2), txtlen, g_rep_chars);
        auto newsize = WideCharToMultiByte(CP_ACP, 0, (wchar_t*)(buff + 2), txtlen, 0, 0, 0, 0);
        MemMan ansi(newsize);
        WideCharToMultiByte(CP_ACP, 0, (wchar_t*)(buff + 2), txtlen, (char*)ansi.Get(), ansi.GetSize(), 0, 0);
        return ansi;
    }
    else
    {
        return MemMan(str);
    }
}

shared_ptr<MemMan> CombineScript(uint8_t* scBuff, uint32_t scSize, vector<string>& ls)
{
    if (ls[ls.size() - 1] == "")
    {
        ls.pop_back();
    }

    uint32_t totalLen = 0;
    uint32_t inputStrCnt = 0;
    for_each(ls.begin(), ls.end(), [&](string& s) {
        if (s.length() > 0 && s[0] == '$')
            return;
        totalLen += s.length();
        inputStrCnt++;
    });
    
    uint32_t strCnt, codeSize, strSize;
    uint32_t codeOffset, strOffset;
    if (memcmp(scBuff, "ESCR1_00", 8) != 0 ||
        (strCnt = *(uint32_t*)(scBuff + 8)) != inputStrCnt + 1)
    {
        return shared_ptr<MemMan>(nullptr);
        //return make_shared<MemMan>(nullptr);
    }

    shared_ptr<MemMan> newSc(new MemMan(scSize + totalLen));
    auto buff = (uint8_t*)newSc->Get();

    codeOffset = 8 + 4 + strCnt * 4 + 4;
    codeSize = *(uint32_t*)(scBuff + codeOffset - 4);
    strOffset = codeOffset + codeSize + 4;
    strSize = *(uint32_t*)(scBuff + strOffset - 4);
    memcpy(buff, scBuff, strOffset);

    auto strOffTbl = (uint32_t*)(buff + 12);
    auto strs = buff + strOffset;
    auto strsBegin = strs;
    *strs++ = 0;
    *strOffTbl++ = 0;
    for_each(ls.begin(), ls.end(), [&](string& s) {
        if (s.length() > 0 && s[0] == '$')
            return;
        *strOffTbl++ = strs - strsBegin;
        memcpy(strs, s.c_str(), s.length());
        strs += s.length();
        *strs++ = 0;
    });
    *(uint32_t*)(buff + strOffset - 4) = strs - strsBegin;
    newSc->SetSize(strs - buff);
    return newSc;
}

typedef ExtFileInfo* (__cdecl *OpenExtFileRoutine)(char* filename);
ExtFileInfo* HOOKFUNC MyOpenFile(OpenExtFileRoutine oldFunc, char* fileName)
{
    auto info = (ExtFileInfo*)oldFunc(fileName);
    if (!info || !info->isMem)
    {
        return info;
    }
    string fname(fileName);
    if (fname.find("script\\") == 0)
    {
        auto pos = fname.find(".bin");
        auto txtName = fname.substr(0, pos) + ".txt";
        MyFileReader reader;
        auto mem = reader.ReadToMem(txtName.c_str());
        if (!mem.get())
        {
            return info;
        }
        auto ansi = CvtToAnsi(*mem);
        auto ls = SplitTxt(ansi);
        auto newScript = CombineScript((uint8_t*)info->buff, info->size, ls);
        if (!newScript.get())
        {
            Log("%s combile error.", fileName);
            MessageBox(0, L"combile error", 0, 0);
            return info;
        }
        auto newInfo = AllocFileInfo(*newScript);
        if (newInfo)
        {
            FreeFileInfo(info);
            return newInfo;
        }
    }
    return info;
}

int HOOKFUNC mystrcmpi(char* s1, char* s2)
{
    return CompareStringA(1041, NORM_IGNORECASE, s1, strlen(s1), s2, strlen(s2)) - 2;
}
typedef BOOL (*TextOutARoutine)(
    HDC hdc,           // デバイスコンテキストのハンドル
    int nXStart,       // 開始位置（基準点）の x 座標
    int nYStart,       // 開始位置（基準点）の y 座標
    LPCSTR lpString,  // 文字列
    int cbString       // 文字数
);

HFONT create_font()
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = 200;
    lf.lfWeight = 0x200;
    lf.lfQuality = 3;
    lf.lfPitchAndFamily = 1;
    wcscpy_s(lf.lfFaceName, L"Segoe UI Symbol");
    auto hf = CreateFontIndirect(&lf);
    return hf;
}

BOOL HOOKFUNC my_textout(TextOutARoutine old_proc,
    HDC hdc,
    int nXStart,
    int nYStart,
    LPCSTR lpString,
    int cbString
)
{
    static auto sun_font = create_font();
    if (cbString == 2)
    {
        for (auto& rep : g_rep_chars)
        {
            if (rep.ac[0] == lpString[0] && rep.ac[1] == lpString[1])
            {
                wchar_t s[2];
                s[0] = rep.uc;
                s[1] = 0;
                auto old = SelectObject(hdc, sun_font);
                auto ret = TextOutW(hdc, nXStart, nYStart, s, 1);
                SelectObject(hdc, old);
                return ret;
            }
        }
    }
    return old_proc(hdc, nXStart, nYStart, lpString, cbString);
}

BOOL WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    if (_Reason == DLL_PROCESS_ATTACH)
    {
        PatchMemory();
        
        auto buff = (BYTE*)VirtualAlloc(0, 1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        HookSrcObject src;
        HookStubObject stub;
        if (!InitializeHookSrcObject(&src, (void*)0x508610) ||
            !InitializeStubObject(&stub, buff, 100, 0, STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX) ||
            !Hook32(&src, 0, &stub, MyOpenFile, "f1"))
        {
            MessageBox(0, L"无法hook函数2", 0, 0);
            return FALSE;
        }

        auto mod = GetModuleHandle(L"kernel32.dll");
        auto addr = GetProcAddress(mod, "lstrcmpiA");
        if (!InitializeHookSrcObject(&src, (void*)addr, true) ||
            !InitializeStubObject(&stub, buff + 100, 900, 8, STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX) ||
            !Hook32(&src, 0, &stub, mystrcmpi, "12"))
        {
            MessageBox(0, L"无法hook函数3", 0, 0);
            return FALSE;
        }

        mod = GetModuleHandle(L"gdi32.dll");
        addr = GetProcAddress(mod, "TextOutA");
        if (!InitializeHookSrcObject(&src, (void*)addr, true) ||
            !InitializeStubObject(&stub, buff + 200, 100, 20, STUB_DIRECTLYRETURN | STUB_OVERRIDEEAX) ||
            !Hook32(&src, 0, &stub, my_textout, "f12345"))
        {
            MessageBox(0, L"无法hook函数4", 0, 0);
            return FALSE;
        }
    }
    return TRUE;
}