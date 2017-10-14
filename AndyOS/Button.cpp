#include "Button.h"
#include "Window.h"
#include "drawing.h"
#include "debug.h"

namespace gui
{
	bool hover = 0;
	bool pressed = 0;

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

		if (hover)
			Drawing::DrawRect(0, 0, gc.width, gc.height, 1, COLOR_BLACK, gc);

		if (pressed)
			Drawing::DrawRect(0, 0, gc.width, gc.height, 1, COLOR_RED, gc);
	}

	void Button::ReceiveSendMessage(WND_MSG msg)
	{
		switch (msg.type)
		{
		case WM_MOUSEENTER:
			hover = 1;
			break;

		case WM_MOUSELEAVE:
			hover = 0;
			break;

		case WM_MOUSEDOWN:
			pressed = 1;
			break;

		case WM_MOUSEUP:
			pressed = 0;
			break;

		default:
			break;
		}
	}
}
