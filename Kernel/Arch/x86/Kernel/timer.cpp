#include "Arch/timer.h"
#include "Arch/pit.h"

namespace Timer::Arch
{
    uint64 Ticks()
    {
        return (uint64)PIT::ticks * 1000;
    }
}