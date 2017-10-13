#pragma once
#include "drawing.h"

namespace gui
{
	class Window;

	enum CONTROL_TYPE
	{
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

		Rect bounds;

		CONTROL_TYPE type;

		Window* parent;
		Control* next;
		Control* previous;

		virtual void Draw() {};
	};

	class Label : public Control
	{
	public:
		char* text;

		Label();

		virtual void Draw();
	};
}