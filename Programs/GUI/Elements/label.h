#pragma once
#include <string.h>
#include "andyos/string.h"
#include "element.h"

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