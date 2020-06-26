#include <AndyOS.h>
#include "textbox.h"

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

		gc.Clear(background);
		gc.DrawRect(0, 0, bounds.width, bounds.height, 1, Color::Black);
		gc.DrawText(2, 2, text.c_str(), foreground, background);

        int time = get_ticks() - focusTime;

        if (isActive)
        {
            if (time % blinkTime < blinkTime / 2)
            {
                int x = text.length() * 8 + 2;
                gc.FillRect(x, 2, 1, 16, foreground);
            }
        }
	}

    void TextBox::Focus()
    {
        focusTime = get_ticks();
    }

    void TextBox::KeyPress(KEY_PACKET packet)
    {
        if (packet.code == KEY_BACK)
        {
            if (text.length() > 0)
            {
                text.pop_back();
                focusTime = get_ticks();
            }
        }
        else
        {
            if (packet.character)
            {
                text += packet.character;
                focusTime = get_ticks();
            }
        }
    }
}
