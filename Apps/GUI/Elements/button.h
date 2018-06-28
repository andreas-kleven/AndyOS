#pragma once
#include "element.h"
#include "string.h"

namespace gui
{
	class Button : public Element
	{
	public:
		String text;

		Button(String text);

		virtual void Paint();
		virtual void MouseDown();
		virtual void MouseUp();
	};
}