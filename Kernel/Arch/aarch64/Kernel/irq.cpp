#include "Arch/irq.h"

namespace IRQ::Arch
{
    bool Install(int num, void(*handler)())
    {
        return false;
    }
}