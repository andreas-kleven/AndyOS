#pragma once
#include <Arch/irqfuncs.h>

namespace IRQ
{
    bool Install(int num, void(*handler)());
}