#pragma once

#include <stdint.h>

class NakedMemory
{
public:
    NakedMemory() :buff_(nullptr), size_(0) {}
    NakedMemory(uint32_t size);
    NakedMemory(const void* buff, uint32_t size);
    NakedMemory(const NakedMemory& other);
    NakedMemory(NakedMemory&& other) noexcept;
    ~NakedMemory();

    void* Get() const
    {
        return buff_;
    }
    uint32_t GetSize() const
    {
        return size_;
    }

    void SetSize(uint32_t size);

    NakedMemory& operator=(const NakedMemory& other);
    NakedMemory& operator=(NakedMemory&& other) noexcept;

private:
    uint8_t* buff_;
    uint32_t size_;
};