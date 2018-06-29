#pragma once
#include "element.h"
#include "string.h"

namespace gui
{
	class TextBox : public Element
	{
	public:
		String text;

		TextBox();

		virtual void Paint();

        virtual void Focus();
        virtual void KeyPress(KEYCODE key);
	};
}