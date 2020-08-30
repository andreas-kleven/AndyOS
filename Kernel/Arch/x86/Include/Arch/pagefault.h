#pragma once
#include <Arch/regs.h>

namespace PageFault::Arch
{
    void HandlePageFault(REGS *regs);
};
