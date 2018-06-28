#include "button.h"

namespace gui
{
	Button::Button(String text)
		: Element(0, 0, 100, 20)
	{
		this->text = text;
		foreground = Color::Black;
		background = Color::LightGray;
	}

	void Button::Paint()
	{
		GC gc = CreateGC();

		Color color_hover = Color(0.9, 0.9, 0.9);
		Color bg_color = isHovering ? color_hover : background;

		Drawing::Clear(bg_color, gc);
		Drawing::DrawRect(0, 0, bounds.width, bounds.height, 1, Color::Black, gc);
		Drawing::DrawText(2, 2, text.ToChar(), foreground, bg_color, gc);
	}
}
