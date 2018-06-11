#include "window.h"
#include "definitions.h"

static int new_id = 1;

Window::Window()
{
	color_background = Color(0, 0.5, 0.5);
	color_foreground = Color::White;
	color_title = Color::Black;

	id = new_id++;

	state = WINDOW_STATE_NORMAL;
	focused = 0;

	gc = GC(bounds.width, bounds.height);

	bounds.x = 20;
	bounds.y = 20;
	bounds.width = gc.width / 2;
	bounds.height = gc.height / 2;

	int cw = gc.width - GUI_WINDOW_BORDER_WIDTH * 2;
	int ch = gc.height - GUI_TITLEBAR_HEIGHT - GUI_WINDOW_BORDER_WIDTH;
	gc_content = GC(gc, GUI_WINDOW_BORDER_WIDTH, GUI_TITLEBAR_HEIGHT, cw, ch);
}


void Window::Paint()
{
	
}

void Window::Close()
{
	
}

void Window::PaintWindow()
{
	Color _titlebar = focused ? color_background : Color::White;
	Color _border = focused ? color_background : Color::Black;

	Drawing::FillRect(0, 0, bounds.width, GUI_TITLEBAR_HEIGHT, _titlebar, gc); //Title bar
	Drawing::DrawText(6, 6, title, color_title, gc); //Title

	Drawing::FillRect(0, GUI_TITLEBAR_HEIGHT, bounds.width, bounds.height, color_foreground, gc); //Content rect

	Drawing::DrawRect(0, 0, bounds.width, bounds.height, GUI_WINDOW_BORDER_WIDTH, _border, gc); //Window border
}


void Window::SetFocus(bool focus)
{
	if (focus != this->focused)
	{
		this->focused = focus;
		Paint();
	}
}

void Window::SetState(WINDOW_STATE state)
{
	switch (state)
	{
	case WINDOW_STATE_NORMAL:
		bounds.width = gc.width / 2;
		bounds.height = gc.height / 2;
		break;

	case WINDOW_STATE_MAXIMIZED:
		bounds.width = gc.width;
		bounds.height = gc.height - 50;
		break;

	case WINDOW_STATE_MINIMIZED:
		break;
	}

	Paint();
}