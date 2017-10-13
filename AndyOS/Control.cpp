#include "Control.h"
#include "Window.h"
#include "drawing.h"

namespace gui
{
	Label::Label()
	{
		type = CONTROL_TYPE_LABEL;
	}

	void Label::Paint()
	{
		Drawing::DrawText(0, 0, text, 0xFF0000, parent->gc);
	}
}
