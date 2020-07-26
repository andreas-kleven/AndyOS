#include <irq.h>
#include <Arch/irq.h>

namespace IRQ
{
    bool Install(int num, void(*handler)())
    {
        return Arch::Install(num, handler);
    }
}