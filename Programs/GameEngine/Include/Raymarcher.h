#pragma once
#include <AndyOS.h>
#include "Game.h"

class Raymarcher
{
    int max_res;
    int res;

public:
    Raymarcher();
	void Render(GC& gc);
};