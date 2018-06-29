#include "window.h"
#include "definitions.h"
#include "string.h"

static int new_id = 1;

Window::Window(int proc_id, char* title, int width, int height, bool capture, uint32* framebuffer)
{
	color_background = Color(0, 0.5, 0.5);
	color_foreground = Color::White;
	color_title = Color::Black;

	this->id = new_id++;
	this->proc_id = proc_id;
	this->capture = capture;

	this->title = new char[strlen(title) + 1];
	strcpy(this->title, title);

	state = WINDOW_STATE_NORMAL;
	focused = false;
	dirty = true;

	bounds.x = id * 100;
	bounds.y = id * 100;
	bounds.width = width;
	bounds.height = height;

	gc = GC(width - GUI_WINDOW_BORDER_WIDTH * 2, height - GUI_TITLEBAR_HEIGHT, framebuffer);
}

void Window::Paint(GC& main_gc)
{
	//if (dirty)
		Drawing::BitBlt(gc, 0, 0, gc.width, gc.height, main_gc, bounds.x + GUI_WINDOW_BORDER_WIDTH, bounds.y + GUI_TITLEBAR_HEIGHT);

	Color _titlebar = focused ? color_background : Color::White;
	Color _border = focused ? color_background : Color::Black;

	Drawing::FillRect(bounds.x, bounds.y, bounds.width, GUI_TITLEBAR_HEIGHT, _titlebar, main_gc); //Title bar
	Drawing::DrawText(bounds.x + 6, bounds.y + 6, title, color_title, main_gc); //Title
	Drawing::DrawRect(bounds.x, bounds.y, bounds.width, bounds.height, GUI_WINDOW_BORDER_WIDTH, _border, main_gc); //Window border

	dirty = false;
}

void Window::Close()
{
	
}

void Window::SetFocus(bool focus)
{
	if (focus != this->focused)
	{
		this->focused = focus;
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
}