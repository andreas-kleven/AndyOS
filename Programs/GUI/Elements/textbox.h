#pragma once
#include <string>
#include "element.h"

namespace gui
{
	class TextBox : public Element
	{
	public:
		std::string text;

		TextBox();

		virtual void Paint();

        virtual void Focus();
        virtual void KeyPress(KEY_PACKET packet);
	};
}