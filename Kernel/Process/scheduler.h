#pragma once
#include "definitions.h"
#include "process.h"
#include "thread.h"

static class Scheduler
{
public:
	static STATUS Init();
	static Thread* CreateKernelThread(void* main);
	static Thread* CreateUserThread(void* main, void* stack);
	static void InsertThread(Thread* thread);

	static void StartThreading();
	static void RemoveThread(Thread* thread);

	static void Exit(int exitcode);

private:
	static void Schedule();
	static void Task_ISR();

	static void Idle();
};