#include "textbox.h"
#include <AndyOS.h>

namespace gui
{
    const int blinkTime = 500;

    static int focusTime = 0;

	TextBox::TextBox()
		: Element(0, 0, 140, 20)
	{
		foreground = Color::Black;
		background = Color::White;
	}

	void TextBox::Paint()
	{
		GC gc = CreateGC();

		Drawing::Clear(background, gc);
		Drawing::DrawRect(0, 0, bounds.width, bounds.height, 1, Color::Black, gc);
		Drawing::DrawText(2, 2, text.ToChar(), foreground, background, gc);

        int time = get_ticks() - focusTime;

        if (isActive)
        {
            if (time % blinkTime < blinkTime / 2)
            {
                int x = text.Length() * 8 + 2;
                Drawing::FillRect(x, 2, 1, 16, foreground, gc);
            }
        }
	}

    void TextBox::Focus()
    {
        focusTime = get_ticks();
    }

    void TextBox::KeyPress(KEYCODE key)
    {
        if (key == KEY_BACK)
        {
            if (text.Length() > 0)
            {
                text = text.Remove(text.Length() - 2);
                focusTime = get_ticks();
            }
        }
        else
        {
            text += (char)key;
            focusTime = get_ticks();
        }        
    }
}
