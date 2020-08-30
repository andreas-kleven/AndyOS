#pragma once
#include <types.h>

struct REGS
{
    uint32 gs, fs, es, ds;
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32 eip, cs, eflags;
    uint32 user_stack, user_ss;
} __attribute__((packed));