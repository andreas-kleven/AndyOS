#include "Control.h"
#include "drawing.h"

namespace gui
{
	Label::Label()
	{
		type = CONTROL_TYPE_LABEL;
	}

	void Label::Draw()
	{
		Drawing::DrawText(bounds.x, bounds.y, text, 0xFF0000);
	}
}
