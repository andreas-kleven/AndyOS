#include "GUI.h"
#include "drawing.h"
#include "task.h"
#include "mouse.h"
#include "hal.h"
#include "Debug.h"

#define TASKBAR_HEIGHT		40

namespace gui
{
	uint32 col_taskbar = 0x7F7F7F;

	GC gc_background;
	GC gc_taskbar;
	GC gc_cursor;

	int window_count;
	Window* first_window;
	Window* last_window;

	int active_window = 0;

	int cursor_x = 0;
	int cursor_y = 0;

	STATUS WindowManager::Init()
	{
		gc_background = GC::CreateGraphics(Drawing::gc.width, Drawing::gc.height);
		gc_taskbar = GC::CreateGraphics(Drawing::gc.width, TASKBAR_HEIGHT);
		gc_cursor = GC::CreateGraphics(10, 10);
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

			PaintDesktop();
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

	void WindowManager::PaintDesktop()
	{
		Drawing::FillRect(0, 0, Drawing::gc.width, Drawing::gc.height, 0x7F, gc_background);
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
		Drawing::FillRect(0, 0, gc_cursor.width, gc_cursor.height, 0xFFFFFF, gc_cursor);
		Drawing::DrawRect(0, 0, gc_cursor.width, gc_cursor.height, 1, 0, gc_cursor);
		Drawing::BitBlt(gc_cursor, 0, 0, gc_cursor.width, gc_cursor.height, Drawing::gc, cursor_x, cursor_y);
	}

	void WindowManager::HandleMouseInput()
	{
		cursor_x = Mouse::x;
		cursor_y = Mouse::y;

		if (Mouse::mouse_L)
		{
			Window* win = GetWindowAtCursor();

			if (win)
			{
				SetActiveWindow(win->id);
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

	void WindowManager::SetActiveWindow(int id)
	{
		active_window = id;

		Window* win = first_window;
		while (win)
		{
			if (win->id == id)
				win->SetActive(1);
			else if (win->active)
				win->SetActive(0);

			win = win->next;
		}
	}
}