#pragma once
#include <stdint.h>
#include <memory>
#include "NakedMemory.h"

class IInterface
{
public:
    virtual void Dispose() = 0;
};

class IFileReader :public IInterface
{
public:
    virtual NakedMemory ReadToMem(const char* fileName) = 0;
    virtual NakedMemory ReadToMem(const wchar_t* fileName) = 0;
};

class MyFileReader :public IFileReader
{
public:
    MyFileReader() {}
    virtual ~MyFileReader()
    {
    }
    NakedMemory ReadToMem(const char* fileName) override;
    NakedMemory ReadToMem(const wchar_t* fileName) override;
    void Dispose() override
    {
        delete this;
    }

private:
};

bool exists_file(const char* file_name);
bool exists_file(const wchar_t* file_name);
