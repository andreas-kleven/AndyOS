#include "TextEdit.h"
#include "GUI.h"
#include "string.h"
#include "keyboard.h"

using namespace gui;

void apps::RunTextEdit()
{
	Window* win = WindowManager::CreateWindow("TextEdit");

	Label* lbl = new Label();
	lbl->text = "Hello";

	win->AddControl(lbl);

	std::String text = lbl->text;

	while (1)
	{
		Debug::x = 0;
		Debug::y = 0;

		KEY_PACKET pkt = Keyboard::GetLastKey();
		if (pkt.key && pkt.pressed)
		{
			Keyboard::DiscardLastKey();
			text += pkt.character;
			lbl->text = text.ToChar();
		}

		//lbl->x = Mouse::x;
		//lbl->y = Mouse::y;

		_asm pause
	}
}
