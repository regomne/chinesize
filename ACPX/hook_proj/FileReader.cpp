#include "FileReader.h"
#include <windows.h>
#include <stdio.h>

std::shared_ptr<MemMan> MyFileReader::ReadToMem(const char* fileName)
{
    auto fp = fopen(fileName, "rb");
    if (!fp)
    {
        return std::shared_ptr<MemMan>(nullptr);
    }

    fseek(fp, 0, SEEK_END);
    auto fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    std::shared_ptr<MemMan> mem(new MemMan(fsize));
    auto buff = mem->Get();

    auto bytesRead = fread(buff, 1, fsize, fp);
    fclose(fp);
    if (bytesRead != fsize)
    {
        return false;
    }
    return mem;
}
