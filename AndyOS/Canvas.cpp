#include "Canvas.h"
#include "Window.h"
#include "drawing.h"
#include "debug.h"

namespace gui
{
	Canvas::Canvas()
	{
		type = CONTROL_TYPE_CANVAS;
		separate_gc = 1;
	}
}
