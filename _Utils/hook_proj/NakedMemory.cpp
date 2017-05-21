#include "NakedMemory.h"
#include <memory.h>

NakedMemory::NakedMemory(uint32_t size)
{
    buff_ = new uint8_t[size];
    size_ = size;
}

NakedMemory::NakedMemory(const void* buff, uint32_t size)
{
    buff_ = new uint8_t[size];
    size_ = size;
    memcpy(buff_, buff, size);
}

NakedMemory::NakedMemory(const NakedMemory& other)
{
    *this = other;
}

NakedMemory::NakedMemory(NakedMemory&& other) noexcept
{
    buff_ = other.buff_;
    size_ = other.size_;

    other.buff_ = nullptr;
    other.size_ = 0;
}

NakedMemory& NakedMemory::operator=(const NakedMemory& other)
{
    buff_ = new uint8_t[other.size_];
    size_ = other.size_;
    memcpy(buff_, other.buff_, size_);
    return *this;
}

NakedMemory& NakedMemory::operator=(NakedMemory&& other) noexcept
{
    if (this != &other)
    {
        delete[] buff_;

        buff_ = other.buff_;
        size_ = other.size_;

        other.buff_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

NakedMemory::~NakedMemory()
{
    if (buff_)
    {
        delete[] buff_;
    }
}

void NakedMemory::SetSize(uint32_t size)
{
    if (size_ >= size)
    {
        size_ = size;
    }
    else
    {
        auto newBuff = new uint8_t[size];
        if (buff_)
        {
            memcpy(newBuff, buff_, size_);
            delete[] buff_;
        }
        buff_ = newBuff;
        size_ = size;
    }
}
