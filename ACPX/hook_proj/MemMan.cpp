#include "memman.h"
#include <memory.h>

MemMan::MemMan(uint32_t size)
{
    buff_ = new uint8_t[size];
    size_ = size;
}

MemMan::MemMan(const void* buff, uint32_t size)
{
    buff_ = new uint8_t[size];
    size_ = size;
    memcpy(buff_, buff, size);
}

MemMan::MemMan(const MemMan& other)
{
    buff_ = new uint8_t[other.size_];
    size_ = other.size_;
    memcpy(buff_, other.buff_, size_);
}

MemMan::~MemMan()
{
    delete[] buff_;
}

void MemMan::SetSize(uint32_t size)
{
    if (size_ >= size)
    {
        size_ = size;
    }
    else
    {
        auto newBuff = new uint8_t[size];
        memcpy(newBuff, buff_, size_);
        delete[] buff_;
        buff_ = newBuff;
        size_ = size;
    }
}
