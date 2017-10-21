#pragma once

static class Panic
{
public:
	static void KernelPanic(char* err, char* msg = "", ...);
};