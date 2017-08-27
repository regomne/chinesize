#pragma once

#include <stdint.h>
#include <string>

struct PatchStruct
{
    char* mod_name;
    uint32_t offset;
    char* pattern;
    char* hex;
    uint32_t len;
};

struct HookPointStruct
{
    char* module_name;
    uint32_t offset;
    void* hook_routine;
    char* reg_tags;
    uint32_t options;
    union
    {
        uint32_t ret_value;
        uint32_t dest_rva;
    };
};
struct HookPointStructWithName
{
    char* module_name;
    char* proc_name;
    void* hook_routine;
    char* reg_tags;
    uint32_t options;
    union
    {
        uint32_t ret_value;
        uint32_t dest_rva;
    };
};

uint32_t ThiscallFunction0(void* addr, void* this_p);

uint32_t ThiscallFunction1(void* addr, void* thisp, uint32_t arg1);

uint32_t ThiscallFunction2(void* addr, void* thisp, uint32_t arg1, uint32_t arg2);

uint32_t ThiscallFunction3(void* addr, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3);

uint32_t ThiscallFunction4(void* addr, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

void Log(wchar_t* format, ...);

void Log(char* format, ...);

#ifdef LOG_DETAIL
#define LOGERROR(...) Log(__VA_ARGS__)
#define LOGINFO(...) Log(__VA_ARGS__)
#else
#define LOGERROR(...) Log(__VA_ARGS__)
#define LOGINFO(...)
#endif

bool PatchMemory(PatchStruct* psts, uint32_t cnt);

bool HookFunctions(const HookPointStruct* hooks, uint32_t cnt);
bool HookFunctions(const HookPointStructWithName* hooks, uint32_t cnt);

template <size_t _Size>
bool HookFunctions(const HookPointStruct(&strt)[_Size])
{
    return HookFunctions(strt, _Size);
}
template <size_t _Size>
bool HookFunctions(const HookPointStructWithName(&strt)[_Size])
{
    return HookFunctions(strt, _Size);
}

std::wstring decode_string(const char* s, int cp);
std::wstring decode_string(const char* s, uint32_t len, int cp);
