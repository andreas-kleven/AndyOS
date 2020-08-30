#include <Arch/idt.h>
#include <Arch/irq.h>

namespace IRQ::Arch {
bool Install(int num, void (*handler)())
{
    return IDT::InstallIRQ(num, handler);
}
} // namespace IRQ::Arch