#include "Arch/init.h"
#include "Arch/gdt.h"
#include "Arch/tss.h"
#include "Arch/pic.h"
#include "Arch/idt.h"
#include "Arch/pit.h"
#include "Arch/cpu.h"
#include "Arch/syscalls.h"

#include "hal.h"

#include "exceptions.h"

namespace Kernel::Arch
{
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
}