#pragma once

class Panic
{
public:
	static void KernelPanic(char* err, char* msg = "", ...);
};