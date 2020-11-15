#include <Arch/pit.h>
#include <Arch/timer.h>

namespace Timer::Arch {
uint64 Ticks()
{
    return (uint64)PIT::ticks * 10000;
}
} // namespace Timer::Arch
