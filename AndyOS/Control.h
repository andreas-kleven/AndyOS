#pragma once
#include "drawing.h"

namespace gui
{
	class Window;

	enum WINDOW_MESSAGE_TYPE
	{
		WM_NONE,
		WM_CLOSE,
		WM_PAINT,
		WM_CLICK

	};

	struct WINDOW_MESSAGE
	{
		int id;
		WINDOW_MESSAGE_TYPE type;
		int wParam;
		int lParam;

		WINDOW_MESSAGE()
		{ }

		WINDOW_MESSAGE(int id, WINDOW_MESSAGE_TYPE type, int wParam, int lParam)
		{
			this->id = id;
			this->type = type;
			this->wParam = wParam;
			this->lParam = lParam;
		}
	};

	enum CONTROL_TYPE
	{
		CONTROL_TYPE_NONE,
		CONTROL_TYPE_LABEL,
		CONTROL_TYPE_BUTTON,
		CONTROL_TYPE_TEXTBOX,
		CONTROL_TYPE_RICHTEXT,
		CONTROL_TYPE_CHECKBOX
	};

	class Control
	{
	public:
		int id;
		CONTROL_TYPE type;

		char* name;

		Rect bounds;
		GC gc;

		Window* parent;
		Control* next;
		Control* previous;

		uint32 background = 0;
		uint32 foreground = 0;

		virtual void Paint() {};

		void ReceiveSentMessage();

	private:
		void SendMessage();
	};
}