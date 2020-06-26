#pragma once
#include <string>
#include "element.h"

namespace gui
{
	class Label : public Element
	{
	public:
		std::string text;

		Label(const std::string& text);

		virtual void Paint();
	};
}