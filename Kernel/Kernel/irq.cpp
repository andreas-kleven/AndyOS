#include "irq.h"
#include "HAL/idt.h"

namespace IRQ
{
    bool Install(int num, IRQ_HANDLER handler)
    {
        return Arch::Install(num, handler);
    }
}