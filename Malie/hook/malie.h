#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <functional>
#include "ilhook.h"

class MalieString
{
public:
    MalieString(const std::wstring& str) :str_(str) {}
    bool init();
    std::vector<std::wstring> export_str();
    std::wstring import_str(std::function<std::wstring(void)> get_str);

private:
    enum class State {
        Init,
        Voice,
        Ruby,
        Text,
    };

    typedef std::pair<int, int> StrPos;

private:
    std::wstring str_;
    std::vector<StrPos> str_pos_;
};

void HOOKFUNC MyReadStrTbl(Registers* regs);
DWORD WINAPI malie_init_thread(LPVOID param);
void HOOKFUNC MyReadVmData(Registers* regs);
void HOOKFUNC MyReadDataString(const wchar_t** s_ptr);
void HOOKFUNC MyReadLibp(Registers* regs);

extern CRITICAL_SECTION g_init_lock;
