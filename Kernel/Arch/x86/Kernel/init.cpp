#include "exceptions.h"
#include <Arch/cpu.h>
#include <Arch/gdt.h>
#include <Arch/idt.h>
#include <Arch/init.h>
#include <Arch/pic.h>
#include <Arch/pit.h>
#include <Arch/syscalls.h>
#include <Arch/tss.h>
#include <hal.h>

namespace Kernel::Arch {
void Init()
{
    disable();

    CPU::Init();
    GDT::Init();
    TSS::Init(5, 0x9000);
    PIC::Init();
    IDT::Init();
    PIT::Init();

    Exceptions::Init();
    Syscalls::Arch::Init();

    enable();
}
} // namespace Kernel::Arch