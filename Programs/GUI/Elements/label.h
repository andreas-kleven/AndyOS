#pragma once
#include "element.h"
#include "string.h"

namespace gui
{
	class Label : public Element
	{
	public:
		String text;

		Label(String text);

		virtual void Paint();
	};
}