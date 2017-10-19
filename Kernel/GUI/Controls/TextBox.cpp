#include "TextBox.h"
#include "GUI/GUI.h"

namespace gui
{
	TextBox::TextBox()
	{
		type = CONTROL_TYPE_TEXTBOX;
		background = COLOR_GRAY;
		foreground = COLOR_BLACK;
	}

	void TextBox::Paint()
	{
		Drawing::FillRect(0, 0, bounds.width, bounds.height, background, gc);
		Drawing::DrawRect(0, 0, bounds.width, bounds.height, 1, COLOR_BLACK, gc);
		Drawing::DrawText(0, 0, text.ToChar(), foreground, gc);
	}

	void TextBox::ReceiveSendMessage(WND_MSG msg)
	{
		switch (msg.type)
		{
		case WM_KEY:
			text += msg.lParam;
			break;
		}
	}
}