#include <Arch/pagefault.h>
#include <Arch/scheduler.h>
#include <Process/scheduler.h>
#include <memory.h>
#include <debug.h>
#include "exceptions.h"

#define PAGE_FAULT_PRESENT (1 << 0)
#define PAGE_FAULT_WRITE (1 << 1)
#define PAGE_FAULT_USER (1 << 2)
#define PAGE_FAULT_RESERVED_WRITE (1 << 3)
#define PAGE_FAULT_INSTRUCTION_FETCH (1 << 4)

namespace PageFault::Arch
{
    static int fault_addr = 0;

    void HandlePageFault(REGS *regs)
    {
        asm("mov %%cr2, %%eax\n"
            "mov %%eax, (%0)"
            : "=m"(fault_addr));

        uint32 err = Exceptions::error_code;

        debug_print("Page fault %p %p %p\n", regs, err, fault_addr);

        const char *msg1 = (err & PAGE_FAULT_PRESENT) ? "Page-protection violation  " : "None-present page  ";
        const char *msg2 = (err & PAGE_FAULT_WRITE) ? "Write  " : "Read  ";
        const char *msg3 = (err & PAGE_FAULT_USER) ? "User mode  " : "Kernel mode  ";
        const char *msg4 = (err & PAGE_FAULT_RESERVED_WRITE) ? "Reserved write  " : "";
        const char *msg5 = (err & PAGE_FAULT_INSTRUCTION_FETCH) ? "Instruction fetch  " : "";

        if ((err & PAGE_PRESENT) && (err & PAGE_FAULT_USER))
        {
            if (VMem::PerformCopy((void *)fault_addr))
                return;
        }

        if (!(err & PAGE_FAULT_USER))
            regs->user_stack = 0;

        panic("Page fault", "ADDR:%X  ERR:%X  EFLAGS:%X  CS:%X  EIP:%X  ESP:%X  EBP:%X\nEAX:%X  EBX:%X  ECX:%X  EDX:%X  ESI:%X  EDI:%X  %s%s%s%s%s\nUser ESP:%X",
              fault_addr, err, regs->eflags, regs->cs, regs->eip, regs->esp, regs->ebp, regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi, msg1, msg2, msg3, msg4, msg5, regs->user_stack);
    }
}; // namespace PageFault::Arch
