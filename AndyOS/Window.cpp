#include "Window.h"
#include "Debug.h"

namespace gui
{
	Window::Window()
	{
		state = WINDOW_STATE_DEFAULT;
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
		DrawWindow();
		DrawControls();
	}

	void Window::DrawWindow()
	{
		Drawing::DrawRect(0, 0, bounds.width, bounds.height, 0xFFFFFF, gc);
		Drawing::DrawRect(0, 0, bounds.width, 28, 0x7F7F7F, gc);
		Drawing::DrawText(6, 6, title, 0, gc);
	}

	void Window::DrawControls()
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