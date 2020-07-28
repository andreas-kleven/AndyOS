#pragma once
#include <time.h>

namespace RTC
{
	int Second();
	int Minute();
	int Hour();
	int Day();
	int Month();
	int Year();
	struct tm Time();
};