#include "timer.h"
#include "Arch/timer.h"
#include "hal.h"

namespace Timer
{
    size_t Ticks()
    {
        return Arch::Ticks();
    }
    
	void Sleep(size_t time)
	{
		size_t end = Ticks() + time;
		while (Ticks() < end) pause();
	}
}