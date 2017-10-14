#include "Window.h"
#include "Debug.h"

#include "Label.h"
#include "Button.h"

namespace gui
{
	static int new_id = 1;

	uint32 col_background = 0xFF008080;
	uint32 col_foreground = COLOR_WHITE;
	uint32 col_title = COLOR_BLACK;

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
		gc_content = GC(gc, GUI_WINDOW_BORDER_WIDTH, GUI_TITLEBAR_HEIGHT, cw, ch);
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
		if (children_count == 0)
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


	Control* Window::CreateControl(CONTROL_TYPE type, char* name, int x, int y, int width, int height, int id)
	{
		Control* ctrl;

		switch (type)
		{
		case CONTROL_TYPE_NONE:
			return 0;

		case CONTROL_TYPE_LABEL:
			ctrl = new Label();
			break;

		case CONTROL_TYPE_BUTTON:
			ctrl = new Button();
			break;

			/*case CONTROL_TYPE_TEXTBOX:
			break;
			case CONTROL_TYPE_RICHTEXT:
			break;
			case CONTROL_TYPE_CHECKBOX:
			break;*/

		default:
			return 0;
		}

		ctrl->type = type;
		ctrl->type = type;
		ctrl->name = name;
		ctrl->bounds = Rect(x, y, width, height);
		ctrl->id = id;

		ctrl->parent = this;
		ctrl->next = 0;
		ctrl->previous = 0;

		ctrl->gc = GC(gc_content, ctrl->bounds);

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
		return ctrl;
	}

	Control* Window::GetControlAt(int x, int y)
	{
		if (children_count == 0)
			return 0;

		x -= bounds.x + gc_content.x;
		y -= bounds.y + gc_content.y;

		Control* ctrl = first_child;
		while (ctrl)
		{
			if (ctrl->bounds.Contains(x, y))
				return ctrl;

			ctrl = ctrl->next;
		}

		return 0;
	}


	WINDOW_MESSAGE Window::GetMessage()
	{
		while (!b_message) 
			_asm pause

		b_message = 0;
		return message;
	}

	void Window::ReceiveSendMessage(WINDOW_MESSAGE msg)
	{
		message = msg;
		b_message = 1;
	}
}