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
};

class MyFileReader :public IFileReader
{
public:
    MyFileReader() {}
    ~MyFileReader()
    {
    }
    NakedMemory ReadToMem(const char* fileName) override;
    void Dispose() override
    {
        delete this;
    }

private:
};

bool exists_file(const char* file_name);
