#include "GUI.h"
#include "drawing.h"
#include "task.h"
#include "mouse.h"
#include "Debug.h"

namespace gui
{
	int window_count;
	Window* first_window;

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

		}
		else
		{
			first_window = window;
		}

		window_count++;
		return window;
	}

	void WindowManager::UpdateLoop()
	{
		while (1)
		{
			first_window->bounds.x = Mouse::x;
			first_window->bounds.y = Mouse::y;

			Drawing::Clear(0);
			DrawDesktop();
			Draw();
			Drawing::Draw();
		}
	}

	void WindowManager::DrawDesktop()
	{

	}

	void WindowManager::Draw()
	{
		if (!window_count)
			return;

		Window* win = first_window;
		while (win)
		{
			win->Paint();

			Rect& bounds = win->bounds;
			GC& src = win->gc;
			Drawing::BitBlt(src, 0, 0, src.width, src.height, Drawing::gc, bounds.x, bounds.y);

			win = win->next;
		}
	}
}