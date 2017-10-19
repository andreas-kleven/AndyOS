#pragma once
#include "Drawing/drawing.h"

class Mandelbrot
{
public:
	Mandelbrot(GC gc_out);
	void Run();

private:
	GC gc_buf;
	GC gc_out;
};