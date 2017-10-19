#include "Canvas.h"
#include "GUI/Window.h"
#include "Drawing/drawing.h"
#include "debug.h"

namespace gui
{
	Canvas::Canvas()
	{
		type = CONTROL_TYPE_CANVAS;
		separate_gc = 1;
	}
}
