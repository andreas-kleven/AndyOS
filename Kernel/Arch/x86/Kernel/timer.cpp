#include "Arch/timer.h"
#include "Arch/pit.h"

namespace Timer::Arch
{
    size_t Ticks()
    {
        return PIT::ticks;
    }
}