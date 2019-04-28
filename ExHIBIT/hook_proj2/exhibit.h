#pragma once

#include <stdint.h>
#include <ilhook.h>

uint32_t HOOKFUNC MyLoadRld(void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2);

uint32_t HOOKFUNC MyProcCmds(Registers* regs, void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3);

void HOOKFUNC MyAddString(Registers* regs);

