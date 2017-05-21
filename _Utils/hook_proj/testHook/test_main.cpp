#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

#include "..\NakedMemory.h"
#include "..\FileReader.h"
#include "..\PicSupport.h"
#include "..\TxtSupport.h"

#include "..\crc32.h"

using namespace std;

struct TextInfo {
    vector<string>* lines;
    vector<uint32_t>* idxs;
};
map<uint32_t, TextInfo> g_TextInfo;

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
        printf("findFile fail\n");
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

void InitWs2() {
    auto txtList = GetFileList(L"txt\\*.txt");
    for (auto& txtName : txtList) {
        auto idxFullName = L"idx\\" + txtName.substr(0, txtName.length() - 4) + L".idx";
        auto txtFullName = L"txt\\" + txtName;
        auto lines = ReadTxt(txtFullName);
        if (!lines) {
            wprintf(L"Reading %s error.\n", txtFullName.c_str());
            continue;
        }
        uint32_t crc;
        auto idxs = ReadIdx(idxFullName, &crc);
        if (!idxs) {
            wprintf(L"Reading %s eror.\n", idxFullName.c_str());
            delete lines;
            continue;
        }
//         TextInfo info = { lines,idxs };
//         g_TextInfo[crc] = info;
    }
}
struct ArcHeader {
    uint32_t cnt;
    uint32_t idxSize;
};

struct EntryInfo {
    uint32_t size;
    uint32_t off;
};

bool ReadArcFileList(const wchar_t* arcName, map<wstring, wstring>& fileMap) {
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
        fileMap[s.str()] = arcName;
    }
    fclose(fp);
    return true;
}
int main()
{
    wstringstream ss;
    ss.str(L"#23424 บวบว");
    wchar_t c;
    int idx;
    ss.seekg(1, 0);
    ss >> idx;
    wcout << ' ' << idx << endl;
    ss.str(L"#26771 ");
    ss.seekg(1, 0);
    ss >> idx;
    cout << idx << endl;
}
