#include "acpx.h"
#include <string>
#include <memory>
#include <algorithm>
#include "FileReader.h"
#include "FuncHelper.h"
#include "TxtSupport.h"

using namespace std;
int HOOKFUNC MyStrcmpi(const char* s1, const char* s2) {
    return CompareStringA(1041, NORM_IGNORECASE, s1, strlen(s1), s2, strlen(s2)) - 2;
}

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

ExtFileInfo* AllocFileInfo(const NakedMemory& mem)
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

unique_ptr<NakedMemory> CombineScript(uint8_t* scBuff, uint32_t scSize, vector<string>& ls)
{
    if (ls[ls.size() - 1] == "")
    {
        ls.pop_back();
    }

    uint32_t totalLen = 0;
    uint32_t inputStrCnt = 0;
    //for_each(ls.begin(), ls.end(), [&](string& s) {
    //    if (s.length() > 0 && s[0] == '$')
    //        return;
    //    totalLen += s.length();
    //    inputStrCnt++;
    //    });

    for (const auto& l : ls) {
        if (l.length() == 0 || l[0] != '$') {
            totalLen += l.length();
            inputStrCnt++;
        }
    }

    uint32_t strCnt, codeSize, strSize;
    uint32_t codeOffset, strOffset;
    if (memcmp(scBuff, "ESCR1_00", 8) != 0 ||
        (strCnt = *(uint32_t*)(scBuff + 8)) != inputStrCnt + 1)
    {
        return nullptr;
    }

    unique_ptr<NakedMemory> newSc(new NakedMemory(scSize + totalLen));
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
    //for_each(ls.begin(), ls.end(), [&](string& s) {
    //    if (s.length() > 0 && s[0] == '$')
    //        return;
    //    *strOffTbl++ = strs - strsBegin;
    //    memcpy(strs, s.c_str(), s.length());
    //    strs += s.length();
    //    *strs++ = 0;
    //    });

    for (const auto& l : ls) {
        if (l.length() == 0 || l[0] != '$') {
            *strOffTbl++ = strs - strsBegin;
            memcpy(strs, l.c_str(), l.length());
            strs += l.length();
            *strs++ = 0;
        }
    }

    *(uint32_t*)(buff + strOffset - 4) = strs - strsBegin;
    newSc->SetSize(strs - buff);
    return newSc;
}

ExtFileInfo* HOOKFUNC MyOpenFile(OpenExtFileRoutine oldFunc, const char* fileName)
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
        if (!mem)
        {
            return info;
        }
        auto ansi = CvtToAnsi(mem);
        auto ls = SplitTxtA(ansi);
        auto newScript = CombineScript((uint8_t*)info->buff, info->size, ls);
        if (!newScript.get())
        {
            Log("%s combile error.", fileName);
            MessageBoxW(0, L"combile error", 0, 0);
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


//HFONT create_font()
//{
//    LOGFONT lf;
//    memset(&lf, 0, sizeof(lf));
//    lf.lfHeight = 200;
//    lf.lfWeight = 0x200;
//    lf.lfQuality = 3;
//    lf.lfPitchAndFamily = 1;
//    wcscpy_s(lf.lfFaceName, L"Segoe UI Symbol");
//    auto hf = CreateFontIndirect(&lf);
//    return hf;
//}
//
//BOOL HOOKFUNC MyTextout(TextOutARoutine old_proc,
//    HDC hdc,
//    int nXStart,
//    int nYStart,
//    LPCSTR lpString,
//    int cbString
//)
//{
//    static auto sun_font = create_font();
//    if (cbString == 2)
//    {
//        for (auto& rep : g_rep_chars)
//        {
//            if (rep.ac[0] == lpString[0] && rep.ac[1] == lpString[1])
//            {
//                wchar_t s[2];
//                s[0] = rep.uc;
//                s[1] = 0;
//                auto old = SelectObject(hdc, sun_font);
//                auto ret = TextOutW(hdc, nXStart, nYStart, s, 1);
//                SelectObject(hdc, old);
//                return ret;
//            }
//        }
//    }
//    return old_proc(hdc, nXStart, nYStart, lpString, cbString);
//}
