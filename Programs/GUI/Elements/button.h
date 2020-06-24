#pragma once
#include <string.h>
#include "andyos/string.h"
#include "element.h"

namespace gui
{
	class Button : public Element
	{
	public:
		String text;

		Button(String text);

		void (*OnClick)() = 0;

		virtual void Paint();
		virtual void MouseDown();
		virtual void MouseUp();
	};
}