#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include "NakedMemory.h"

class MyPack {
private:
#pragma pack(1)
    struct FileEntry {
        char fname[51];
        uint32_t offset;
        uint32_t size;
    };
#pragma pack()

public:
    MyPack(const std::wstring& packName) :packName_(packName) {}
    
    bool Init();

    NakedMemory ReadAFile(const std::string& fileName);

private:
    void GetFileList(NakedMemory& mem);
    void XorDec(void* dst, uint32_t len);

private:
    std::wstring packName_;
    std::map<std::string, FileEntry> fileInfos_;
    NakedMemory pack_;
};