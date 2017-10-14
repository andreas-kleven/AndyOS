#include "TextEdit.h"
#include "GUI.h"
#include "string.h"
#include "stdio.h"
#include "keyboard.h"
#include "task.h"

#include "../3DGame/3DGame.h"
#include "Mandelbrot.h"

using namespace gui;

#define ID_LBL_1	100
#define ID_LBL_2	101

#define ID_BTN_1	200
#define ID_BTN_2	201

#define ID_TXT_1	300

#define ID_CANVAS	900

namespace apps
{
	Canvas* tmp1 = 0;
	Canvas* tmp2 = 0;
	void UpdateCanvas()
	{
		Canvas* canvas;
		canvas = tmp2 ? tmp2 : tmp1;

		//Drawing::FillRect(0, 0, 300, 300, COLOR_MAGENTA, canvas->gc);

		if (canvas == tmp1)
		{
			Mandelbrot mandelbrot(canvas->gc);
			mandelbrot.Run();
		}
		else
		{
			MyGame* game = new MyGame();
			GEngine* engine = new GEngine();
			engine->StartGame(game, canvas->gc);
		}

		while (1);
	}

	void RunTextEdit()
	{
		Window* wnd = WindowManager::CreateWindow("Title");

		//Canvas* canvas = (Canvas*)wnd->CreateControl(CONTROL_TYPE_CANVAS, "", 10, 10, 300, 300, ID_CANVAS);
		Label* lbl1 = (Label*)wnd->CreateControl(CONTROL_TYPE_LABEL, "Label", 100, 50, 100, 16, ID_LBL_1);
		Label* lbl2 = (Label*)wnd->CreateControl(CONTROL_TYPE_LABEL, "", 100, 100, 100, 16, ID_LBL_2);
		Button* btn1 = (Button*)wnd->CreateControl(CONTROL_TYPE_BUTTON, "Button", 10, 10, 80, 20, ID_BTN_1);
		Button* btn2 = (Button*)wnd->CreateControl(CONTROL_TYPE_BUTTON, "Exit", 100, 10, 80, 20, ID_BTN_2);
		TextBox* txt1 = (TextBox*)wnd->CreateControl(CONTROL_TYPE_TEXTBOX, "", 300, 10, 200, 20, ID_TXT_1);

		char buf[256];

		/*if (!tmp1)
			tmp1 = canvas;
		else
			tmp2 = canvas;

		Task::InsertThread(Task::CreateThread(UpdateCanvas));
		//UpdateCanvas();*/

		//void* p;
		//__asm { mov[p], offset update }
		//
		//Task::InsertThread(Task::CreateThread(p));

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
					if (!strcmp(lbl1->name, "Label"))
					{
						wnd->title = "Another title";
						lbl1->name = "Clicked";
						lbl1->foreground = 0xFFFF0000;
					}
					else
					{
						lbl1->name = "Label";
						lbl1->foreground = 0xFF000000;
					}
					break;

				case ID_BTN_2:
					WindowManager::CloseWindow(wnd);
					break;
				}
				break;

			case WM_CLOSE:
				wnd->Close();
				break;
			}

			vprintf(buf, "%i %i", msg.type, msg.id);
			lbl2->name = buf;
		}

	//update:
		//Mandelbrot mandelbrot(canvas->gc);
		//mandelbrot.Run();

		//MyGame* game = new MyGame();
		//GEngine* engine = new GEngine();
		//engine->StartGame(game, canvas->gc);

	/*std::String text = lbl1->text;

	while (1)
	{
		Debug::x = 0;
		Debug::y = 0;

		KEY_PACKET pkt = Keyboard::GetLastKey();
		if (pkt.key && pkt.pressed)
		{
			Keyboard::DiscardLastKey();
			text += pkt.character;
			lbl1->text = text.ToChar();
		}

		//lbl1->x = Mouse::x;
		//lbl1->y = Mouse::y;

		_asm pause
	}*/
	}
}