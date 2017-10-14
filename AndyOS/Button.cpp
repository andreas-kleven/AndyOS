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
		background = COLOR_GRAY;
	}

	void Button::Paint()
	{
		Drawing::FillRect(0, 0, gc.width, gc.height, background, gc);
		Drawing::DrawText(0, 0, name, foreground, gc);

		if (pressed)
			Drawing::DrawRect(0, 0, gc.width, gc.height, 1, COLOR_RED, gc);
		else if (hover)
			Drawing::DrawRect(0, 0, gc.width, gc.height, 1, COLOR_GREEN, gc);
		else
			Drawing::DrawRect(0, 0, gc.width, gc.height, 1, COLOR_BLACK, gc);
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
