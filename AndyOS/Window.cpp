#include "Window.h"
#include "Debug.h"

namespace gui
{
	Window::Window()
	{
		state = WINDOW_STATE_DEFAULT;
		bounds.x = 20;
		bounds.y = 20;
		bounds.width = Drawing::screen.bounds.width / 2;
		bounds.height = Drawing::screen.bounds.height / 2;
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

	void Window::Draw()
	{
		DrawWindow();
		DrawControls();
	}

	void Window::DrawWindow()
	{
		Drawing::DrawRect(bounds.x, bounds.y, bounds.width, bounds.height, 0xFFFFFF);
		Drawing::DrawRect(bounds.x, bounds.y, bounds.width, 28, 0x7F7F7F);
		Drawing::DrawText(bounds.x + 6, bounds.y + 6, title, 0);
	}

	void Window::DrawControls()
	{
		if (!children_count)
			return;

		Control* ctrl = first_child;
		while (ctrl)
		{
			ctrl->Draw();
			ctrl = ctrl->next;
		}
	}
}