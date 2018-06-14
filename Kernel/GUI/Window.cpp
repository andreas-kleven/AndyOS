#include "Window.h"
#include "Lib/debug.h"
#include "HAL/hal.h"

#include "Controls/Label.h"
#include "Controls/Button.h"
#include "Controls/TextBox.h"
#include "Controls/Canvas.h"

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

		gc = GC(bounds.width, bounds.height);

		int cw = gc.width - GUI_WINDOW_BORDER_WIDTH * 2;
		int ch = gc.height - GUI_TITLEBAR_HEIGHT - GUI_WINDOW_BORDER_WIDTH;
		gc_content = GC(gc, GUI_WINDOW_BORDER_WIDTH, GUI_TITLEBAR_HEIGHT, cw, ch);
	}


	void Window::Paint()
	{
		PaintWindow();
		PaintControls();
	}

	void Window::Close()
	{
		/*if (child_count == 0)
			return;

		Control* ctrl = first_child;
		while (ctrl)
		{
			ctrl->Close();
			Control* next = ctrl->next;

			delete ctrl;
			ctrl = next;
		}

		child_count = 0;
		first_child = 0;
		last_child = 0;

		delete &msg_queue;*/
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
		if (child_count == 0)
			return;

		Control* ctrl = last_child;
		while (ctrl)
		{
			ctrl->Paint();

			if (ctrl->separate_gc)
			{
				Rect bounds = ctrl->bounds;
				Drawing::BitBlt(ctrl->gc, 0, 0, bounds.width, bounds.height, gc_content, bounds.x, bounds.y);
			}

			ctrl = ctrl->previous;
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
		/*Control* ctrl;

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

		case CONTROL_TYPE_TEXTBOX:
			ctrl = new TextBox();
			((TextBox*)ctrl)->text = name;
			break;

			/*case CONTROL_TYPE_RICHTEXT:
			break;
			case CONTROL_TYPE_CHECKBOX:
			break;*/

		/*case CONTROL_TYPE_CANVAS:
			ctrl = new Canvas();
			break;

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

		if (ctrl->separate_gc)
		{
			ctrl->gc = GC(clamp(ctrl->bounds.width, 0, gc_content.width),
				clamp(ctrl->bounds.height, 0, gc_content.height));
		}
		else
		{
			ctrl->gc = GC(gc_content, ctrl->bounds);
		}

		if (child_count)
		{
			first_child->previous = ctrl;
			ctrl->next = first_child;
			first_child = ctrl;
		}
		else
		{
			first_child = ctrl;
			last_child = ctrl;
		}

		child_count++;
		return ctrl;*/
	}

	Control* Window::GetControlAt(int x, int y)
	{
		if (child_count == 0)
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


	WND_MSG Window::GetMessage()
	{
		WND_MSG msg;
		while (!msg_queue.Pop(msg))
			asm("pause");

		return msg;
	}

	void Window::ReceiveSendMessage(WND_MSG msg)
	{
		msg_queue.Push(msg);
	}

	void Window::DispatchMessage(WND_MSG msg)
	{
		Control* ctrl = first_child;
		while (ctrl)
		{
			if (ctrl->id == msg.id || msg.id == 0)
				ctrl->ReceiveSendMessage(msg);

			ctrl = ctrl->next;
		}
	}
}