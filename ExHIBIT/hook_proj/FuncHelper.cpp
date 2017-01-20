#include "FuncHelper.h"
#include <windows.h>
#include <stdio.h>

__declspec(naked) uint32_t ThiscallFunction0(void* addr, void* this_p)
{
    __asm
    {
        mov ecx, [esp + 8];
        mov eax, [esp + 4];
        call eax;
        ret;
    }
}

__declspec(naked) uint32_t ThiscallFunction1(void* addr, void* thisp, uint32_t arg1)
{
    __asm
    {
        push ebp;
        mov ebp, esp;
        push arg1;
        mov ecx, thisp;
        call addr;
        pop ebp;
        ret;
    }
}
__declspec(naked) uint32_t ThiscallFunction2(void* addr, void* thisp, uint32_t arg1, uint32_t arg2)
{
    __asm
    {
        push ebp;
        mov ebp, esp;
        push arg2;
        push arg1;
        mov ecx, thisp;
        call addr;
        pop ebp;
        ret;
    }
}

__declspec(naked) uint32_t ThiscallFunction3(void* addr, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    __asm
    {
        push ebp;
        mov ebp, esp;
        push arg3;
        push arg2;
        push arg1;
        mov ecx, thisp;
        call addr;
        pop ebp;
        ret;
    }
}

__declspec(naked) uint32_t ThiscallFunction4(void* addr, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    __asm
    {
        push ebp;
        mov ebp, esp;
        push arg4;
        push arg3;
        push arg2;
        push arg1;
        mov ecx, thisp;
        call addr;
        pop ebp;
        ret;
    }
}

void Log(wchar_t* format, ...)
{
    wchar_t buffer[0x1000];
    va_list ap;
    va_start(ap, format);
    auto char_cnt = vswprintf_s(buffer, format, ap);
    FILE* fp = nullptr;
    auto err = fopen_s(&fp, "log.log", "ab+");
    fwrite(buffer, 1, char_cnt * 2, fp);
    fwrite(L"\r\n", 1, 4, fp);
    fclose(fp);
    va_end(ap);
}

void Log(char* format, ...)
{
    char buffer[0x1000];
    va_list ap;
    va_start(ap, format);
    auto char_cnt = vsprintf_s(buffer, format, ap);
    FILE* fp = nullptr;
    auto err = fopen_s(&fp, "log.log", "ab+");
    fwrite(buffer, 1, char_cnt, fp);
    fwrite("\r\n", 1, 2, fp);
    fclose(fp);
    va_end(ap);
}