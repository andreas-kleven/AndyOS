#include "Button.h"
#include "Window.h"
#include "drawing.h"
#include "debug.h"

namespace gui
{
	Button::Button()
	{
		type = CONTROL_TYPE_BUTTON;

		foreground = COLOR_BLACK;
		background = COLOR_CYAN;
	}

	void Button::Paint()
	{
		Drawing::FillRect(0, 0, gc.width, gc.height, background, gc);
		Drawing::DrawText(0, 0, name, foreground, gc);
	}
}
