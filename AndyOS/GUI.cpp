#include "GUI.h"
#include "drawing.h"
#include "task.h"
#include "mouse.h"
#include "hal.h"
#include "Debug.h"
#include "iso.h"

namespace gui
{
	uint32 cursor_bitmap[8 * 14] =
	{
		0xFF000000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF000000, 0xFF000000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00, 0x00, 0x00,
		0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00, 0x00,
		0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00,
		0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00,
		0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
		0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFF000000, 0x00,
		0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00, 0x00,
		0xFF000000, 0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xFF000000, 0xFF000000, 0xFF000000, 0x00, 0x00
	};

	uint32 col_taskbar = 0xFFD0D0D0;
	uint32 col_desktop_bg = 0xFF003399;

	GC gc_background;
	GC gc_taskbar;
	GC gc_cursor;

	BMP* bmp_background;

	int window_count;
	Window* first_window;
	Window* last_window;

	Window* focused_window;

	int cursor_x = 0;
	int cursor_y = 0;

	bool mouse_click_L = 0;
	bool mouse_click_R = 0;
	bool mouse_click_M = 0;
	bool window_drag = 0;

	MOUSE_CLICK_INFO mouse_click_L_info;
	MOUSE_CLICK_INFO mouse_click_R_info;
	MOUSE_CLICK_INFO mouse_click_M_info;
	WINDOW_DRAG_INFO window_drag_info;

	Window* hover_window = 0;
	Control* hover_control = 0;

	STATUS WindowManager::Init()
	{
		gc_background = GC::CreateGraphics(Drawing::gc.width, Drawing::gc.height);
		gc_taskbar = GC::CreateGraphics(Drawing::gc.width, GUI_TASKBAR_HEIGHT);
		gc_cursor = GC::CreateGraphics(8, 14);

		//char* img_buf;
		//if (!ISO_FS::ReadFile("img_hd.bmp", img_buf))
		//	return STATUS_FAILED;
		//bmp_background = new BMP(img_buf);

		return STATUS_SUCCESS;
	}

	void WindowManager::Start()
	{
		Thread* thread = Task::CreateThread(UpdateLoop);
		Task::InsertThread(thread);
	}

	Window* WindowManager::CreateWindow(char* title)
	{
		Window* window = new Window();
		window->title = title;

		window->next = 0;
		window->previous = 0;

		if (window_count)
		{
			window->previous = last_window;
			last_window->next = window;
			last_window = window;
		}
		else
		{
			first_window = window;
			last_window = window;
		}

		window_count++;
		return window;
	}

	void WindowManager::CloseWindow(Window* wnd)
	{
		wnd->Close();

		if (wnd->next)
			wnd->next->previous = wnd->previous;

		if (wnd->previous)
			wnd->previous->next = wnd->next;

		if (wnd == first_window)
			first_window = wnd->next;

		if (wnd == last_window)
			last_window = wnd->previous;

		delete wnd;
	}

	void WindowManager::UpdateLoop()
	{
		int lastTicks = PIT::ticks;
		int sticks = PIT::ticks;
		int tickcount = 0;

		while (1)
		{
			Debug::Clear(0);
			Drawing::Clear(0);

			HandleMouseInput();

			PaintBackground();
			PaintWindows();
			PaintTaskbar();
			PaintCursor();
			Drawing::Draw();

			int ticks = PIT::ticks;
			int delta = ticks - lastTicks;
			int delta2 = ticks - sticks;

			tickcount++;

			if (delta2 != 0)
				Debug::Print("Delta: %i\tFPS: %i", delta, tickcount * 1000 / delta2);

			if (delta2 > 1000)
			{
				sticks = ticks;
				tickcount = 0;
			}

			lastTicks = ticks;
		}
	}


	void WindowManager::PaintBackground()
	{
		Drawing::FillRect(0, 0, Drawing::gc.width, Drawing::gc.height, col_desktop_bg, gc_background);
		//Drawing::DrawImage(0, 0, gc_background.width, gc_background.height, bmp_background, gc_background);
		Drawing::BitBlt(gc_background, 0, 0, gc_background.width, gc_background.height, Drawing::gc, 0, 0);
	}

	void WindowManager::PaintWindows()
	{
		if (window_count == 0)
			return;

		Window* wnd = last_window;
		while (wnd)
		{
			wnd->Paint();

			Rect& bounds = wnd->bounds;
			GC& src = wnd->gc;
			Drawing::BitBlt(src, 0, 0, src.width, src.height, Drawing::gc, bounds.x, bounds.y);

			wnd = wnd->previous;
		}
	}

	void WindowManager::PaintTaskbar()
	{
		int width = gc_taskbar.width;
		int height = gc_taskbar.height;
		int y = Drawing::gc.height - gc_taskbar.height;

		Drawing::FillRect(0, 0, gc_taskbar.width, gc_taskbar.height, col_taskbar, gc_taskbar);
		Drawing::BitBlt(gc_taskbar, 0, 0, gc_taskbar.width, gc_taskbar.height, Drawing::gc, 0, y);
	}

	void WindowManager::PaintCursor()
	{
		for (int y = 0; y < 14; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				uint32 c = cursor_bitmap[x + y * 8];
				if (c != 0)
					Drawing::SetPixel(x, y, c, gc_cursor);
			}
		}

		Drawing::BitBlt(gc_cursor, 0, 0, gc_cursor.width, gc_cursor.height, Drawing::gc, cursor_x, cursor_y, 1);
	}


	void WindowManager::HandleMouseInput()
	{
		cursor_x = Mouse::x;
		cursor_y = Mouse::y;

		int time = PIT::ticks;

		Window* wnd = GetWindowAtCursor();
		Debug::Print("%i\n%i\n", hover_window, wnd->id);

		Control* ctrl = 0;
		if (wnd)
			ctrl = wnd->GetControlAt(cursor_x, cursor_y);


		if (wnd != hover_window)
		{
			if (hover_window)
			{
				hover_window->ReceiveSendMessage(WND_MSG(hover_window->id, WM_MOUSELEAVE, 0, 0));
			}

			if (wnd)
			{
				wnd->ReceiveSendMessage(WND_MSG(wnd->id, WM_MOUSEENTER, 0, 0));
			}

			hover_window = wnd;
		}

		if (ctrl != hover_control)
		{
			if (hover_control)
			{
				hover_window->ReceiveSendMessage(WND_MSG(hover_control->id, WM_MOUSELEAVE, 0, 0));
			}

			if (ctrl)
			{
				wnd->ReceiveSendMessage(WND_MSG(ctrl->id, WM_MOUSEENTER, 0, 0));
			}

			hover_control = ctrl;
		}

		if (Mouse::mouse_L)
		{
			if (!mouse_click_L)
			{
				//Mouse down

				mouse_click_L = 1;
				mouse_click_L_info.click_time = time;
				mouse_click_L_info.click_x = cursor_x;
				mouse_click_L_info.click_y = cursor_y;

				mouse_click_L_info.window = wnd;

				if (wnd)
				{
					mouse_click_L_info.ctrl = ctrl;

					if (wnd->id != focused_window->id)
						SetFocusedWindow(wnd);

					if (ctrl)
						wnd->ReceiveSendMessage(WND_MSG(ctrl->id, WM_MOUSEDOWN, 0, 0));
					else
						wnd->ReceiveSendMessage(WND_MSG(wnd->id, WM_MOUSEDOWN, 0, 0));

				}
			}
			else
			{
				//Drag

				int dx = cursor_x - mouse_click_L_info.click_x;
				int dy = cursor_y - mouse_click_L_info.click_y;

				if (wnd)
				{
					if (!window_drag && (dx != 0 || dy != 0))
					{
						if (Rect(wnd->bounds.x, wnd->bounds.y, wnd->bounds.width, GUI_TITLEBAR_HEIGHT)
							.Contains(mouse_click_L_info.click_x, mouse_click_L_info.click_y))
						{
							window_drag = 1;
							window_drag_info.window = wnd;
							window_drag_info.start_x = wnd->bounds.x;
							window_drag_info.start_y = wnd->bounds.y;
						}
					}
				}

				if (window_drag)
				{
					window_drag_info.window->bounds.x = window_drag_info.start_x + dx;
					window_drag_info.window->bounds.y = window_drag_info.start_y + dy;
				}
			}
		}
		else
		{
			if (mouse_click_L)
			{
				//Mouse up

				MOUSE_CLICK_INFO& info = mouse_click_L_info;

				mouse_click_L = 0;
				info.rel_time = time;
				info.rel_x = cursor_x;
				info.rel_y = cursor_y;

				if (window_drag)
				{
					window_drag = 0;
				}
				else
				{
					if (info.ctrl)
					{
						info.window->ReceiveSendMessage(WND_MSG(info.ctrl->id, WM_MOUSEUP, 0, 0));

						if (info.ctrl == ctrl)
							info.window->ReceiveSendMessage(WND_MSG(info.ctrl->id, WM_CLICK, 0, 0));
					}
				}
			}
		}
	}


	Window* WindowManager::GetWindowAtCursor()
	{
		Window* wnd = first_window;
		while (wnd)
		{
			if (wnd->bounds.Contains(cursor_x, cursor_y))
				return wnd;

			wnd = wnd->next;
		}

		return 0;
	}

	void WindowManager::SetFocusedWindow(Window* new_focused)
	{
		focused_window = new_focused;

		if (!focused_window->focused)
		{
			if (focused_window != first_window)
			{
				if (focused_window == last_window && focused_window->previous)
					last_window = focused_window->previous;

				//Disconnect
				if (focused_window->previous)
					focused_window->previous->next = focused_window->next;
				if (focused_window->next)
					focused_window->next->previous = focused_window->previous;

				//Insert first
				focused_window->next = first_window;
				focused_window->previous = 0;

				if (first_window->next)
					first_window->next->previous = first_window;
				first_window->previous = focused_window;
				first_window = focused_window;
			}

			focused_window->SetFocus(1);
		}

		Window* wnd = first_window;
		while (wnd)
		{
			if (wnd != focused_window && wnd->focused)
			{
				wnd->SetFocus(0);
			}

			wnd = wnd->next;
		}
	}
}