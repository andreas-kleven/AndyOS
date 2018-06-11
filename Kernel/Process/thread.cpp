#include "thread.h"
#include "Memory/memory.h"
#include "string.h"
#include "debug.h"

void THREAD::Sleep(uint32 until)
{
	if (this->state != THREAD_STATE_TERMINATED)
	{
		this->state = THREAD_STATE_BLOCKING;
		this->sleep_until = until;
	}
}
