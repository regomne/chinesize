#pragma once

#include <stdint.h>
#include "ilhook.h"

ptrdiff_t HOOKFUNC my_read_pic(Registers* regs, ptrdiff_t old_pos);