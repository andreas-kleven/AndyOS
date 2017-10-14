#include "TextEdit.h"
#include "GUI.h"
#include "string.h"
#include "keyboard.h"

using namespace gui;

#define ID_LBL_1 100
#define ID_BTN_1 101

namespace apps
{
	void RunTextEdit()
	{
		Window* wnd = WindowManager::CreateWindow("TextEdit");

		Label* lbl = (Label*)wnd->CreateControl(CONTROL_TYPE_LABEL, "Hello", 100, 50, 100, 16, ID_LBL_1);
		Button* btn = (Button*)wnd->CreateControl(CONTROL_TYPE_BUTTON, "Button", 10, 10, 80, 20, ID_BTN_1);

		while (1)
		{
			WND_MSG msg = wnd->GetMessage();
			wnd->DispatchMessage(msg);

			switch (msg.type)
			{
			case WM_CLICK:
				switch (msg.id)
				{
				case ID_BTN_1:
					if (!strcmp(btn->name, "Button"))
						btn->name = "Clicked";
					else
						btn->name = "Button";
					break;
				}
			}
		}

		/*std::String text = lbl->text;

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
		}*/
	}
}