#pragma once
#include <stdint.h>
#include <memory>
#include "memman.h"

class IInterface
{
public:
    virtual void Dispose() = 0;
};

class IFileReader :public IInterface
{
public:
    virtual std::shared_ptr<MemMan> ReadToMem(const char* fileName) = 0;
};

class MyFileReader :public IFileReader
{
public:
    MyFileReader() {}
    ~MyFileReader()
    {
    }
    std::shared_ptr<MemMan> ReadToMem(const char* fileName) override;
    void Dispose() override
    {
        delete this;
    }

private:
};
