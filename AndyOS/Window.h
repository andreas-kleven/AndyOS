#pragma once
#include "Control.h"

#define GUI_WINDOW_BORDER_WIDTH		1
#define GUI_TITLEBAR_HEIGHT			28

namespace gui
{
	enum WINDOW_STATE
	{
		WINDOW_STATE_NORMAL,
		WINDOW_STATE_MAXIMIZED,
		WINDOW_STATE_MINIMIZED
	};

	class Window
	{
	public:
		int id;

		Rect bounds;

		WINDOW_STATE state;
		bool focused;

		Window* next;
		Window* previous;

		int children_count;
		Control* first_child;
		Control* last_child;

		GC gc;
		GC gc_content;

		char* title;

		Window();

		void AddControl(Control* ctrl);

		void Paint();

	private:
		void PaintWindow();
		void PaintControls();

	public:
		void SetFocus(bool focus);
		void SetState(WINDOW_STATE state);

		Control* GetControlAt(int x, int y);
	};
}