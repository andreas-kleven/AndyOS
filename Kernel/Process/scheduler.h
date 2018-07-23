#pragma once
#include "definitions.h"
#include "process.h"
#include "thread.h"

namespace Scheduler
{
	void Enable();
	void Disable();

	void ExitThread(int code, THREAD* thread);
	void SleepThread(uint32 until, THREAD* thread);
	void BlockThread(THREAD* thread);
	void AwakeThread(THREAD* thread);
	
	void InsertThread(THREAD* thread);
	void RemoveThread(THREAD* thread);

	THREAD* CurrentThread();
	THREAD* Schedule();
	
	void Init();
};