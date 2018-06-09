#include "FileReader.h"
#include <windows.h>
#include <stdio.h>

NakedMemory MyFileReader::ReadToMem(const char* fileName)
{
    FILE* fp = nullptr;
    fopen_s(&fp, fileName, "rb");
    if (!fp)
    {
        return NakedMemory();
    }

    fseek(fp, 0, SEEK_END);
    auto fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    NakedMemory mem(fsize);
    auto buff = mem.Get();

    auto bytesRead = fread(buff, 1, fsize, fp);
    fclose(fp);
    if (bytesRead != fsize)
    {
        return NakedMemory();
    }
    return mem;
}

NakedMemory MyFileReader::ReadToMem(const wchar_t* fileName)
{
    FILE* fp = nullptr;
    _wfopen_s(&fp, fileName, L"rb");
    if (!fp)
    {
        return NakedMemory();
    }

    fseek(fp, 0, SEEK_END);
    auto fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    NakedMemory mem(fsize);
    auto buff = mem.Get();

    auto bytesRead = fread(buff, 1, fsize, fp);
    fclose(fp);
    if (bytesRead != fsize)
    {
        return NakedMemory();
    }
    return mem;
}

bool exists_file(const char* file_name)
{
    auto attr = GetFileAttributesA(file_name);
    return (attr != (DWORD)INVALID_HANDLE_VALUE) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool exists_file(const wchar_t* file_name)
{
    auto attr = GetFileAttributesW(file_name);
    return (attr != (DWORD)INVALID_HANDLE_VALUE) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}
