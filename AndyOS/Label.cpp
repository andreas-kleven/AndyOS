#include "Label.h"
#include "Window.h"
#include "drawing.h"

namespace gui
{
	Label::Label()
	{
		type = CONTROL_TYPE_LABEL;

		foreground = 0xFF000000;
		background = 0;
	}

	void Label::Paint()
	{
		//GC gc = parent->gc_content;

		//Drawing::FillRect(0, 0, gc.width, gc.height, 0xFFFF, gc);
		Drawing::DrawText(0, 0, text, foreground, background, gc);
	}
}
