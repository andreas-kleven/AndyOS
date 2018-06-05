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

	static void Exit(int exitcode);

private:
	static void Schedule();
	static void Task_ISR();

	static void Idle();
};