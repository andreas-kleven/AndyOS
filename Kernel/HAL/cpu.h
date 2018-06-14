#pragma once
#include "definitions.h"

class CPU
{
public:
	static STATUS Init();

private:
	static STATUS EnableSSE();
	static STATUS EnableFPU();
};