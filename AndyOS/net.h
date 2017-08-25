#pragma once
#include "definitions.h"
#include "../Kernel/net.h"

static class Net : public KNet
{
public:
	static STATUS Init();
};