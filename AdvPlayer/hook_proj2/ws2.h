#pragma once
#include "ilhook.h"
#include <stdint.h>

void HOOKFUNC MyOpenFile(Registers* regs);
uint32_t HOOKFUNC MyReadInst_v15(Registers* regs, uint32_t old_proc_func);
uint32_t HOOKFUNC MyReadInst_v17(Registers* regs, uint32_t old_proc_func);
typedef uint32_t(__stdcall* old_read_inst_func)(uint32_t, uint32_t, const char*, uint32_t);
uint32_t HOOKFUNC MyReadInst_v19(Registers* regs, old_read_inst_func old_proc_func, uint32_t a0, uint32_t a1, const char* a2, uint32_t a3);
void HOOKFUNC MyMbtowc(Registers* regs);
void HOOKFUNC MyMbToWc2(int* cp, int* flags, char** str_ptr, uint32_t* str_len);
void InitWs2();
void HOOKFUNC MySelString_v15(Registers* regs);
void HOOKFUNC MySelString_v17(Registers* regs);
void HOOKFUNC MySelString_v19(Registers* regs);
void HOOKFUNC MyOpenFile1(wchar_t* arcName, wchar_t* entName);
void HOOKFUNC MyChangeFont_v15(Registers* regs);
void HOOKFUNC MyChangeFont_v17(const wchar_t** font_name);
