#include "ws2.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

#include "crc32.h"
#include "FileReader.h"
#include "FuncHelper.h"
#include "TxtSupport.h"

using namespace std;

bool g_Ws2HasChanged;

struct TextInfo {
    vector<string>* lines;
    vector<uint32_t>* idxs;
};

map<uint32_t, TextInfo> g_TextInfo;
map<wstring, wstring> g_MyFiles;

#define READ_INST_RETURN_ADDR 0x4c953f
#define OPEN_FILE_ARG1_OFFSET 0xb04
#define NAME_LINE_START "%LF"
#define LINE_END_WITH_WAIT "%K%P"

constexpr wchar_t* ChArcName = L"Ch.arc";

void HOOKFUNC MyOpenFile1(wchar_t* arcName, wchar_t* entName) {
    wstring s = entName;
    transform(s.begin(), s.end(), s.begin(), towlower);
    auto itr = g_MyFiles.find(s);
    if (itr != g_MyFiles.end()) {
        if (wcslen(arcName) >= 6) {
            wcscpy(arcName, itr->second.c_str());
//             Log(L"Pic replaced: %s", entName);
        }
        else {
            Log(L"Replace arc name error:%s\n", arcName);
        }
    }
}

void HOOKFUNC MyOpenFile(Registers* regs) {
    auto fnamePtr = *(wchar_t**)(regs->esp + OPEN_FILE_ARG1_OFFSET);
    if (!fnamePtr) {
        return;
    }
    auto fname = wstring(fnamePtr);
    transform(fname.begin(), fname.end(), fname.begin(), towlower);
    if (fname.length() > 3 && fname.substr(fname.length() - 3) == L"ws2") {
        g_Ws2HasChanged = true;
    }
}

template<class T>
int bin_search(const T* start, uint32_t cnt, const T& val) {
    auto init_start = start;
    auto end = start + cnt;
    
    while (start <= end)
    {
        auto middle = start + (end - start) / 2;
        if (*middle > val) {
            end = middle - 1;
        }
        else if(*middle < val){
            start = middle + 1;
        }
        else {
            return middle - init_start;
        }
    }
    return -1;
}

uint32_t CallProcFunc(uint32_t old_proc_func, int a1, int a2, int a3, int a4, int a5, int a6) {
    uint32_t retVal;
    __asm {
        push a6;
        push a5;
        push a4;
        push a3;
        mov edx, a2;
        mov ecx, a1;
        call old_proc_func;
        mov retVal, eax;
    }
    return retVal;
}

uint32_t HOOKFUNC MyReadInst(Registers* regs, uint32_t old_proc_func) {
    auto a6 = *(int*)(regs->esp + 0x10);
    auto a5 = *(int*)(regs->esp + 0xc);
    auto a4 = *(int*)(regs->esp + 0x8);
    auto a3 = *(int*)(regs->esp + 0x4);
    auto a2 = (int)regs->edx;
    auto a1 = (int)regs->ecx;
    if (*(uint32_t*)regs->esp != READ_INST_RETURN_ADDR) {
        return CallProcFunc(old_proc_func, a1, a2, a3, a4, a5, a6);
    }

    auto scriptObj = (uint32_t*)regs->esi;
    auto scriptStart = (uint8_t*)scriptObj[8];
    auto scriptEnd = (uint8_t*)scriptObj[9];
    auto curOff = scriptObj[4];
    auto scriptSize = scriptEnd - scriptStart;
    
    static TextInfo* txtInfo = nullptr;
    if (g_Ws2HasChanged) {
        g_Ws2HasChanged = false;
        auto crc = crc32(scriptStart, scriptSize > 0x1000 ? 0x1000 : scriptSize);
        auto itr = g_TextInfo.find(crc);
        if (itr != g_TextInfo.end()) {
            txtInfo = &itr->second;
        }
        else {
            txtInfo = nullptr;
        }
    }
    if (txtInfo) {
        auto idx = bin_search(&(*txtInfo->idxs)[0], txtInfo->idxs->size(), curOff);
        auto& lines = *(txtInfo->lines);
        if (idx != -1) {
            string newString = "^8";
            auto oldString = string((char*)a2);
            if (oldString.find(NAME_LINE_START) == 0) {
                newString += NAME_LINE_START + lines[idx];
            }
            else if (oldString.length() >= 4 && oldString.substr(oldString.length() - 4) == LINE_END_WITH_WAIT) {
                newString += lines[idx] + LINE_END_WITH_WAIT;
            }
            else {
                newString += lines[idx];
            }
            CallProcFunc(old_proc_func, a1, (int)newString.c_str(), a3, a4, a5, a6);
            return oldString.length();
        }
    }
    return CallProcFunc(old_proc_func, a1, a2, a3, a4, a5, a6);
}

void HOOKFUNC MyMbtowc(Registers* regs) {
    auto cp = (int*)(regs->esp);
    auto flags = (int*)(regs->esp + 4);
    auto strPtr = (char**)(regs->esp + 8);
    auto strLen = (uint32_t*)(regs->esp + 0xc);

    auto str = *strPtr;
    if (*strLen == -1 || *strLen > 2) {
        if (str[0] == '^' && str[1] == '8') {
            *cp = CP_UTF8;
            *flags = 0;
            *strPtr += 2;
            if (*strLen != -1 && *strLen > 2) {
                *strLen -= 2;
            }
        }
        else {
            *cp = 932;
        }
    }
}

void HOOKFUNC MySelString(Registers* regs) {
    auto scriptObj = (uint32_t*)regs->ebp;
    auto scriptStart = (uint8_t*)scriptObj[8];
    auto scriptEnd = (uint8_t*)scriptObj[9];
    auto curOff = *(uint32_t*)regs->ebx;
    auto scriptSize = scriptEnd - scriptStart;

    TextInfo* txtInfo = nullptr;
    auto crc = crc32(scriptStart, scriptSize > 0x1000 ? 0x1000 : scriptSize);
    auto itr = g_TextInfo.find(crc);
    if (itr != g_TextInfo.end()) {
        txtInfo = &itr->second;
    }
    else {
        txtInfo = nullptr;
    }
    if (txtInfo) {
        auto idx = bin_search(&(*txtInfo->idxs)[0], txtInfo->idxs->size(), curOff);
        auto& lines = *(txtInfo->lines);
        if (idx != -1) {
            auto& line = lines[idx];
            static char newString[200];
            strcpy_s(newString, "^8");
            if (line.find("Sel:") == 0) {
                strcat_s(newString, &line[4]);
            }
            else {
                strcat_s(newString, line.c_str());
            }
            *(char**)(regs->esp) = newString;
        }
    }
}

void HOOKFUNC MyChangeFont(Registers* regs) {
    static const wchar_t* font_name = L"MS UI Gothic";
    regs->ecx = (DWORD)font_name;
}

vector<wstring> GetFileList(wchar_t* path) {
    WIN32_FIND_DATA wfd;
    vector<wstring> files;
    auto hf = FindFirstFile(path, &wfd);
    if (hf != INVALID_HANDLE_VALUE) {
        do {
            files.push_back(wfd.cFileName);
        } while (FindNextFile(hf, &wfd));
        FindClose(hf);
    }
    else {
        Log("findFile fail");
    }
    return std::move(files);
}

vector<string>* ReadTxt(const wstring& txtName) {
    MyFileReader reader;
    auto mem = reader.ReadToMem(txtName.c_str());
    if (!mem.Get()) {
        return nullptr;
    }
    auto ansi = CvtToAnsi(mem, CP_UTF8);
    auto lines = SplitTxtA(ansi);
    while (lines[lines.size() - 1] == "")
    {
        lines.pop_back();
    }
    return new vector<string>(std::move(lines));
}

vector<uint32_t>* ReadIdx(const wstring& idxName, uint32_t* crc) {
    MyFileReader reader;
    auto mem = reader.ReadToMem(idxName.c_str());
    if (!mem.Get()) {
        return nullptr;
    }
    auto buff = (uint32_t*)mem.Get();
    *crc = *buff;
    auto vec = new vector<uint32_t>((mem.GetSize() - 4) / 4);
    memcpy(&(*vec)[0], buff + 1, mem.GetSize() - 4);
    return vec;
}

struct ArcHeader {
    uint32_t cnt;
    uint32_t idxSize;
};

struct EntryInfo {
    uint32_t size;
    uint32_t off;
};

bool ReadArcFileList(const wchar_t* arcName, map<wstring,wstring>& fileMap) {
    FILE* fp = nullptr;
    _wfopen_s(&fp, arcName, L"rb");
    if (!fp) {
        return false;
    }

    ArcHeader hdr;
    fread(&hdr, 1, sizeof(hdr), fp);
    for (uint32_t i = 0;i < hdr.cnt;i++) {
        EntryInfo info;
        fread(&info, 1, sizeof(info), fp);
        
        wstringstream s;
        wchar_t c;
        fread(&c, 1, sizeof(c), fp);
        while (c != 0)
        {
            s << c;
            if (fread(&c, 1, sizeof(c), fp) != sizeof(c)) break;
        }
        auto str = s.str();
        transform(str.begin(), str.end(), str.begin(), towlower);
        fileMap[str] = arcName;
    }
    fclose(fp);
    return true;
}

void InitWs2() {
    auto txtList = GetFileList(L"txt\\*.txt");
    for (auto& txtName : txtList) {
        auto idxFullName = L"idx\\" + txtName.substr(0, txtName.length() - 4) + L".idx";
        auto txtFullName = L"txt\\" + txtName;
        auto lines = ReadTxt(txtFullName);
        if (!lines) {
            Log(L"Reading %s error.", txtFullName.c_str());
            continue;
        }
        uint32_t crc;
        auto idxs = ReadIdx(idxFullName, &crc);
        if (!idxs) {
            Log(L"Reading %s eror.", idxFullName.c_str());
            delete lines;
            continue;
        }
        if (idxs->size() != lines->size()) {
            Log(L"%s line error, need:%d, real:%d", txtFullName.c_str(), idxs->size(), lines->size());
            delete lines;
            delete idxs;
            continue;
        }
        TextInfo info = { lines,idxs };
        g_TextInfo[crc] = info;
    }

    if (!ReadArcFileList(ChArcName, g_MyFiles)) {
        Log(L"Can't find arc:%s", ChArcName);
    }
}