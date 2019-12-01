#pragma once
#include <ilhook.h>

void HOOKFUNC on_dec_ready(char* buffer);
void HOOKFUNC on_dec_complete(Registers* regs);