#pragma once
#include "Message.h"
#include "drawing.h"

namespace gui
{
	class Window;

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

		int child_count = 0;
		Control* first_child = 0;
		Control* last_child = 0;

		uint32 background = 0;
		uint32 foreground = 0;

		virtual void Paint() {};
		virtual void ReceiveSendMessage(WND_MSG msg) {}

		void Close();

	private:
		void SendMessage();
	};
}