#include <Kernel/timer.h>
#include <Arch/timer.h>
#include <hal.h>

namespace Timer
{
    uint64 Ticks()
    {
        return Arch::Ticks();
    }
    
	void Sleep(uint64 time)
	{
		uint64 end = Ticks() + time;
		while (Ticks() < end) pause();
	}
}