#pragma once

#include <stdint.h>

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