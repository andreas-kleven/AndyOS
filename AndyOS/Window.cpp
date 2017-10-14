#include "Window.h"
#include "Debug.h"

namespace gui
{
	static int new_id = 1;

	uint32 col_background = 0xFF008080;
	uint32 col_foreground = COLOR_WHITE;
	uint32 col_title = COLOR_BLACK;

	int control_count;
	Control* first_control;
	Control* last_control;

	Window* focused_control;

	Window::Window()
	{
		id = new_id++;

		state = WINDOW_STATE_NORMAL;
		focused = 0;

		bounds.x = 20;
		bounds.y = 20;
		bounds.width = Drawing::gc.width / 2;
		bounds.height = Drawing::gc.height / 2;

		gc = GC::CreateGraphics(bounds.width, bounds.height);

		int cw = gc.width - GUI_WINDOW_BORDER_WIDTH * 2;
		int ch = gc.height - GUI_TITLEBAR_HEIGHT - GUI_WINDOW_BORDER_WIDTH;
		gc_content = GC(GUI_WINDOW_BORDER_WIDTH, GUI_TITLEBAR_HEIGHT, cw, ch, gc);
	}


	void Window::AddControl(Control* ctrl)
	{
		ctrl->parent = this;
		ctrl->next = 0;
		ctrl->previous = 0;

		ctrl->gc = GC(ctrl->bounds.x, ctrl->bounds.y, ctrl->bounds.width, ctrl->bounds.height, gc_content);

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
		PaintControls();
	}

	void Window::PaintWindow()
	{
		uint32 _titlebar = focused ? col_background : COLOR_WHITE;
		uint32 _border = focused ? col_background : COLOR_BLACK;

		Drawing::FillRect(0, 0, bounds.width, GUI_TITLEBAR_HEIGHT, _titlebar, gc); //Title bar
		Drawing::DrawText(6, 6, title, col_title, gc); //Title

		Drawing::FillRect(0, GUI_TITLEBAR_HEIGHT, bounds.width, bounds.height, col_foreground, gc); //Content rect

		Drawing::DrawRect(0, 0, bounds.width, bounds.height, GUI_WINDOW_BORDER_WIDTH, _border, gc); //Window border
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
			bounds.width = Drawing::gc.width / 2;
			bounds.height = Drawing::gc.height / 2;
			break;

		case WINDOW_STATE_MAXIMIZED:
			bounds.width = Drawing::gc.width;
			bounds.height = Drawing::gc.height - 50;
			break;

		case WINDOW_STATE_MINIMIZED:
			break;
		}

		Paint();
	}


	Control* Window::GetControlAt(int x, int y)
	{
		if (control_count == 0)
			return 0;

		Control* ctrl = first_control;
		while (ctrl)
		{
			if (ctrl->bounds.Contains(x, y))
				return ctrl;

			ctrl = ctrl->next;
		}

		return 0;
	}
}