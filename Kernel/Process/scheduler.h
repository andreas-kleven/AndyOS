#pragma once
#include "types.h"
#include "process.h"
#include "thread.h"

namespace Scheduler
{
	void Enable();
	void Disable();

	void ExitThread(int code, THREAD *thread);
	void SleepThread(uint64 until, THREAD *thread);
	void BlockThread(THREAD *thread, bool auto_switch = true);
	void WakeThread(THREAD *thread);

	void InsertThread(THREAD *thread);
	void RemoveThread(THREAD *thread);

	THREAD *CurrentThread();
	THREAD *Schedule();

	void Init();
	void Start(void (*entry)());
	void Switch();
}; // namespace Scheduler
