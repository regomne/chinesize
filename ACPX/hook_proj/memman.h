#pragma once

#include <stdint.h>

class MemMan
{
public:
    MemMan(uint32_t size);
    MemMan(const void* buff, uint32_t size);
    MemMan(const MemMan& other);
    ~MemMan();

    void* Get() const
    {
        return buff_;
    }
    uint32_t GetSize() const
    {
        return size_;
    }

    void SetSize(uint32_t size);

private:
    uint8_t* buff_;
    uint32_t size_;
};