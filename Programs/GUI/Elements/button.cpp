#include "button.h"

namespace gui
{
	static bool isMouseDown = false;

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

		if (isMouseDown)
			bg_color = Color(0.6, 0.6, 0.6);

		gc.Clear(bg_color);
		gc.DrawRect(0, 0, bounds.width, bounds.height, 1, Color::Black);
		gc.DrawText(2, 2, text.ToChar(), foreground, bg_color);
	}

	void Button::MouseDown()
	{
		isMouseDown = true;
	}

	void Button::MouseUp()
	{
		isMouseDown = false;
		
		if (OnClick)
			OnClick();
	}
}
