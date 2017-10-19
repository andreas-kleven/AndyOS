#pragma once
#include "definitions.h"

static class CPU
{
public:
	static STATUS Init();

private:
	static STATUS EnableSSE();
	static STATUS EnableFPU();
};