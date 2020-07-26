#include <Arch/irq.h>
#include <Arch/idt.h>

namespace IRQ::Arch
{
    bool Install(int num, void(*handler)())
    {
        return IDT::InstallIRQ(num, handler);
    }
}