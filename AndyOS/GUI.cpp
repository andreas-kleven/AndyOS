#include "GUI.h"
#include "drawing.h"
#include "task.h"
#include "mouse.h"
#include "hal.h"
#include "Debug.h"

#define TASKBAR_HEIGHT		40

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

	uint32 col_taskbar = 0xD0D0D0;
	uint32 col_desktop_bg = 0x3399;

	GC gc_background;
	GC gc_taskbar;
	GC gc_cursor;

	int window_count;
	Window* first_window;
	Window* last_window;

	Window* active_window;

	int cursor_x = 0;
	int cursor_y = 0;

	bool mouse_click_L = 0;
	bool mouse_click_R = 0;
	bool mouse_click_M = 0;
	bool window_drag;

	MOUSE_CLICK_INFO mouse_click_L_info;
	MOUSE_CLICK_INFO mouse_click_R_info;
	MOUSE_CLICK_INFO mouse_click_M_info;
	WINDOW_DRAG_INFO window_drag_info;

	STATUS WindowManager::Init()
	{
		gc_background = GC::CreateGraphics(Drawing::gc.width, Drawing::gc.height);
		gc_taskbar = GC::CreateGraphics(Drawing::gc.width, TASKBAR_HEIGHT);
		gc_cursor = GC::CreateGraphics(8, 14);
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

	void WindowManager::UpdateLoop()
	{
		int lastTicks = PIT::ticks;

		while (1)
		{
			Debug::Clear(0);
			Drawing::Clear(0);

			//last_window->bounds.x = Mouse::x;
			//last_window->bounds.y = Mouse::y;
			//last_window->active = 1;

			HandleMouseInput();

			PaintBackground();
			PaintWindows();
			PaintTaskbar();
			PaintCursor();
			Drawing::Draw();

			int ticks = PIT::ticks;
			int delta = ticks - lastTicks;
			if (delta > 0)
				Debug::Print("Delta: %i\tFPS: %i", delta, 1000 / delta);
			lastTicks = ticks;
		}
	}


	void WindowManager::PaintBackground()
	{
		Drawing::FillRect(0, 0, Drawing::gc.width, Drawing::gc.height, col_desktop_bg, gc_background);
		Drawing::BitBlt(gc_background, 0, 0, gc_background.width, gc_background.height, Drawing::gc, 0, 0);
	}

	void WindowManager::PaintWindows()
	{
		if (window_count == 0)
			return;

		Window* win = last_window;
		while (win)
		{
			win->Paint();

			Rect& bounds = win->bounds;
			GC& src = win->gc;
			Drawing::BitBlt(src, 0, 0, src.width, src.height, Drawing::gc, bounds.x, bounds.y);

			win = win->previous;
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

		//Drawing::FillRect(0, 0, gc_cursor.width, gc_cursor.height, 0xFFFFFF, gc_cursor);
		//Drawing::DrawRect(0, 0, gc_cursor.width, gc_cursor.height, 1, 0, gc_cursor);
		Drawing::BitBlt(gc_cursor, 0, 0, gc_cursor.width, gc_cursor.height, Drawing::gc, cursor_x, cursor_y, 1);
	}


	void WindowManager::HandleMouseInput()
	{
		cursor_x = Mouse::x;
		cursor_y = Mouse::y;

		int time = PIT::ticks;

		if (Mouse::mouse_L)
		{
			if (!mouse_click_L)
			{
				//Mouse down

				mouse_click_L = 1;
				mouse_click_L_info.click_time = time;
				mouse_click_L_info.click_x = cursor_x;
				mouse_click_L_info.click_y = cursor_y;

				Window* win = GetWindowAtCursor();

				if (win)
				{
					if (win->id != active_window->id)
						SetActiveWindow(win);


				}
			}
			else
			{
				//Drag

				int dx = cursor_x - mouse_click_L_info.click_x;
				int dy = cursor_y - mouse_click_L_info.click_y;

				if (!window_drag)
				{
					window_drag = 1;
					window_drag_info.start_x = active_window->bounds.x;
					window_drag_info.start_y = active_window->bounds.y;
				}
				else
				{
					active_window->bounds.x = window_drag_info.start_x + dx;
					active_window->bounds.y = window_drag_info.start_y + dy;
				}
			}
		}
		else
		{
			if (mouse_click_L)
			{
				//Mouse up

				mouse_click_L = 0;
				mouse_click_L_info.rel_time = time;
				mouse_click_L_info.rel_x = cursor_x;
				mouse_click_L_info.rel_y = cursor_y;

				if (window_drag)
				{
					window_drag = 0;
				}
			}
		}
	}


	Window* WindowManager::GetWindowAtCursor()
	{
		if (window_count == 0)
			return 0;

		Window* win = first_window;
		while (win)
		{
			if (win->bounds.Contains(cursor_x, cursor_y))
				return win;

			win = win->next;
		}

		return 0;
	}

	void WindowManager::SetActiveWindow(Window* new_active)
	{
		active_window = new_active;

		if (!active_window->active)
		{
			if (active_window != first_window)
			{
				if (active_window == last_window)
					last_window = active_window->previous;

				active_window->previous->next = active_window->next;
				active_window->next = first_window;
				active_window->previous = 0;

				first_window->previous = active_window;
				first_window = active_window;
			}

			active_window->SetActive(1);
		}

		Window* win = first_window;
		while (win)
		{
			if (win != active_window && win->active)
			{
				win->SetActive(0);
			}

			win = win->next;
		}
	}
}