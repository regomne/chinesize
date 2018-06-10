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
static inline uint32_t round_4(uint32_t val)
{
    return (val + 3) & ~3;
}
bool read_bmp_file1(NakedMemory& src, int* width, int* height, int* bit_depth, NakedMemory& dib) {
    auto file_header = (BITMAPFILEHEADER*)src.Get();
    auto info_header = (BITMAPINFOHEADER*)(file_header + 1);
    if (info_header->biBitCount != 24 && info_header->biBitCount != 32) {
        cout << "err1" << endl;
        return false;
    }
    *width = info_header->biWidth;
    *height = info_header->biHeight;
    bool is_bottom = false;
    if (*height < 0) {
        *height = -*height;
        is_bottom = true;
    }
    *bit_depth = info_header->biBitCount;

    auto stride = round_4(*width * (*bit_depth / 8));
    if (file_header->bfOffBits + stride * *height > src.GetSize()) {
        cout << "err2" << endl;
        return false;
    }
    NakedMemory dibdata(stride * *height);
    if (is_bottom) {
        auto p = (uint8_t*)dibdata.Get();
        auto po = (uint8_t*)src.Get() + file_header->bfOffBits;
        p += (*height - 1) * stride;
        for (int i = 0;i < *height;i++) {
            if (*bit_depth == 24) {
                memcpy(p, po, stride);
            }
            else {
                //BGRA -> GRAB
                for (int i = 0;i < *width;i++) {
                    p[i * 4 + 3] = po[i * 4 + 0];
                    p[i * 4 + 0] = po[i * 4 + 1];
                    p[i * 4 + 1] = po[i * 4 + 2];
                    p[i * 4 + 2] = po[i * 4 + 3];
                }
            }
            p -= stride;
            po += stride;
        }
    }
    else {
        auto p = (uint8_t*)dibdata.Get();
        auto po = (uint8_t*)src.Get() + file_header->bfOffBits;
        for (int i = 0;i < *height;i++) {
            if (*bit_depth == 24) {
                memcpy(p, po, stride);
            }
            else {
                //BGRA -> GRAB
                for (int i = 0;i < *width;i++) {
                    p[i * 4 + 0] = po[i * 4 + 3];
                    p[i * 4 + 1] = po[i * 4 + 0];
                    p[i * 4 + 2] = po[i * 4 + 1];
                    p[i * 4 + 3] = po[i * 4 + 2];
                }
            }
            p += stride;
            po += stride;
        }
    }
    dib = std::move(dibdata);
    return true;
}

bool cvt_bmp(string f1, string f2) {
    MyFileReader reader;
    auto bf = reader.ReadToMem(f1.c_str());
    if (!bf.Get()) {
        cout << "read bmp fail" << endl;
        return false;
    }
    int w, h, bit;
    NakedMemory dib;
    if (!read_bmp_file1(bf, &w, &h, &bit, dib)) {
        cout << "read bmp format fail" << endl;
        return false;
    }
    auto fp = fopen(f2.c_str(), "wb");
    auto hd = (BITMAPFILEHEADER*)bf.Get();
    fwrite(bf.Get(), 1, hd->bfOffBits, fp);
    fwrite(dib.Get(), 1, dib.GetSize(), fp);
    fclose(fp);
    return true;
}

int main()
{
    if (!cvt_bmp("title990.bmp", "a.bmp")) {
        cout << "fail" << endl;
    }
    cout << "end" << endl;
}
