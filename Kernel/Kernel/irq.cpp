#include <Arch/irq.h>
#include <irq.h>

namespace IRQ {
bool Install(int num, void (*handler)())
{
    return Arch::Install(num, handler);
}
} // namespace IRQ