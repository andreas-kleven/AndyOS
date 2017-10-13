#pragma once
#include "Control.h"

namespace gui
{
	enum WINDOW_STATE
	{
		WINDOW_STATE_DEFAULT,
		WINDOW_STATE_MAXIMIZED,
		WINDOW_STATE_HIDDEN
	};

	class Window
	{
	public:
		int id;

		Rect bounds;

		WINDOW_STATE state;

		Window* next;
		Window* previous;

		int children_count;
		Control* first_child;
		Control* last_child;

		GC gc;

		char* title;

		Window();

		void AddControl(Control* ctrl);

		void Paint();

	private:
		void DrawWindow();
		void DrawControls();
	};
}