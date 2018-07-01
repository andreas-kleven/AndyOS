#pragma once
#include "definitions.h"
#include "HAL/idt.h"

namespace Mouse
{
	void GetButtons(bool& left, bool& right, bool& middle);

	void GetPos(int& x, int& y);
	void GetScroll(int& x, int& y);

	void ResetPos();
	void ResetScroll();
	
	STATUS Init();
};