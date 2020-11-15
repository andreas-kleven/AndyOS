#include "exceptions.h"
#include <Arch/fpu.h>
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

    FPU::Init();
    GDT::Init();
    TSS::Init(5, 0x9000);
    PIC::Init();
    IDT::Init();
    PIT::Init();

    Exceptions::Init();
    Syscalls::Arch::Init();
    IDT::Enable();

    enable();
}
} // namespace Kernel::Arch
