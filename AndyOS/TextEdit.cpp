#include "TextEdit.h"
#include "GUI.h"
#include "string.h"
#include "stdio.h"
#include "keyboard.h"

using namespace gui;

#define ID_LBL_1	100
#define ID_BTN_1	101
#define ID_BTN_2	102

namespace apps
{
	void RunTextEdit()
	{
		Window* wnd = WindowManager::CreateWindow("TextEdit");

		Label* lbl = (Label*)wnd->CreateControl(CONTROL_TYPE_LABEL, "Hello", 100, 50, 100, 16, ID_LBL_1);
		Button* btn1 = (Button*)wnd->CreateControl(CONTROL_TYPE_BUTTON, "Button", 10, 10, 80, 20, ID_BTN_1);
		//Button* btn2 = (Button*)wnd->CreateControl(CONTROL_TYPE_BUTTON, "Exit", 100, 10, 80, 20, ID_BTN_2);

		char buf[256];

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
					if (!strcmp(lbl->name, "Hello"))
						lbl->name = "Clicked";
					else
						lbl->name = "Hello";
					break;

				case ID_BTN_2:
					WindowManager::CloseWindow(wnd);
					break;
				}

			case WM_PAINT:
				wnd->Close();
				break;
			}

			vprintf(buf, "%i %i", msg.type, msg.id);
			lbl->name = buf;
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