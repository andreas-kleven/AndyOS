#pragma once
#include "definitions.h"
#include "process.h"
#include "thread.h"

static class Scheduler
{
public:
	static THREAD* current_thread;

	static STATUS Init();
	static THREAD* CreateKernelThread(void* main);
	static THREAD* CreateUserThread(void* main, void* stack);
	static void InsertThread(THREAD* thread);

	static void StartThreading();
	static void RemoveThread(THREAD* thread);

	static void SleepThread(uint32 until, THREAD* thread = current_thread);
	static void ExitThread(int code, THREAD* thread = current_thread);

private:
	static void Schedule();
	static void Task_ISR();

	static void Idle();
};