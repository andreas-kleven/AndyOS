#include "Window.h"
#include "Debug.h"

#define WINDOW_BORDER_WIDTH		1
#define TITLEBAR_HEIGHT			28

namespace gui
{
	uint32 col_background = 0x8080;
	uint32 col_foreground = 0xFFFFFF;

	static int new_id = 1;

	Window::Window()
	{
		id = new_id++;

		state = WINDOW_STATE_DEFAULT;
		active = 0;

		bounds.x = 20;
		bounds.y = 20;
		bounds.width = Drawing::gc.width / 2;
		bounds.height = Drawing::gc.height / 2;

		gc = GC::CreateGraphics(bounds.width, bounds.height);
	}

	void Window::AddControl(Control* ctrl)
	{
		ctrl->parent = this;
		ctrl->next = 0;
		ctrl->previous = 0;

		if (children_count)
		{
			last_child->next = ctrl;
			ctrl->previous = last_child;
			last_child = ctrl;
		}
		else
		{
			first_child = ctrl;
			last_child = ctrl;
		}

		children_count++;
	}

	void Window::Paint()
	{
		PaintWindow();
		//PaintControls();
	}

	void Window::SetActive(bool active)
	{
		if (active != this->active)
		{
			this->active = active;
			Paint();
		}
	}

	void Window::PaintWindow()
	{
		uint32 _titlebar = active ? col_background : 0xFFFFFF;
		uint32 _border = active ? col_background : 0;

		Drawing::FillRect(0, 0, bounds.width, TITLEBAR_HEIGHT, _titlebar, gc); //Title bar
		Drawing::DrawText(6, 6, title, 0, gc); //Title

		Drawing::FillRect(0, TITLEBAR_HEIGHT, bounds.width, bounds.height, col_foreground, gc); //Content rect

		Drawing::DrawRect(0, 0, bounds.width, bounds.height, WINDOW_BORDER_WIDTH, _border, gc); //Window border
	}

	void Window::PaintControls()
	{
		if (!children_count)
			return;

		Control* ctrl = first_child;
		while (ctrl)
		{
			ctrl->Paint();
			ctrl = ctrl->next;
		}
	}
}