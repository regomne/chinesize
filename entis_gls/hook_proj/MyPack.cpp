#include "MyPack.h"
#include "FileReader.h"
#include <windows.h>
//#include "pack_key.h"

using namespace std;

bool MyPack::Init() {
    MyFileReader reader;
    auto mem = reader.ReadToMem(packName_.c_str());
    if (!mem.Get()) {
        //Log(L"Can't read pack.");
        return false;
    }
    XorDec(mem.Get(), mem.GetSize());
    GetFileList(mem);
    pack_ = std::move(mem);
    return true;
}

void MyPack::XorDec(void* dst, uint32_t len) {
    auto p = (uint8_t*)dst;
    for (uint32_t i = 0;i < len;i++) {
        //p[i] ^= g_dat_key[i & 1023];
    }
}

static std::wstring decode_string(const char* s, int cp)
{
    auto len = MultiByteToWideChar(cp, 0, s, -1, 0, 0);
    auto buff = new wchar_t[len];
    len = MultiByteToWideChar(cp, 0, s, -1, buff, len);
    std::wstring str(buff);
    delete[] buff;
    return std::move(str);
}

void MyPack::GetFileList(NakedMemory& mem) {
    auto bf = (uint8_t*)mem.Get();
    auto file_cnt = *(uint32_t*)bf;
    auto entries = (FileEntry*)(bf + 4);
    for (uint32_t i = 0;i < file_cnt;i++) {
        fileInfos_[entries[i].fname] = entries[i];
    }
}

NakedMemory MyPack::ReadAFile(const std::string& fileName) {
    auto itr = fileInfos_.find(fileName);
    if (itr != fileInfos_.end()) {
        NakedMemory mem((uint8_t*)pack_.Get() + itr->second.offset, itr->second.size);
        return move(mem);
    }
    else {
        return NakedMemory();
    }
}