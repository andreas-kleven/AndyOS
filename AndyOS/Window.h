#pragma once
#include "Control.h"

#define GUI_WINDOW_BORDER_WIDTH		2
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
		//Control* focused_child;

		GC gc;
		GC gc_content;

		char* title;

		bool b_message = 0;
		WINDOW_MESSAGE message;

		Window();

		void Paint();

		void SetFocus(bool focus);
		void SetState(WINDOW_STATE state);

		Control* CreateControl(CONTROL_TYPE type, char* name, int x, int y, int width, int height, int id);
		Control* GetControlAt(int x, int y);

		WINDOW_MESSAGE GetMessage();
		void ReceiveSendMessage(WINDOW_MESSAGE msg);

	private:
		void PaintWindow();
		void PaintControls();
	};
}