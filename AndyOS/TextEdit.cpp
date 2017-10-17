#include "TextEdit.h"
#include "GUI.h"
#include "string.h"
#include "stdio.h"
#include "keyboard.h"
#include "task.h"

#include "udpsocket.h"

#include "../3DGame/3DGame.h"
#include "Mandelbrot.h"

using namespace gui;

#define ID_LBL_1	100
#define ID_LBL_2	101
#define ID_LBL_3	102

#define ID_BTN_1	200
#define ID_BTN_2	201
#define ID_BTN_3	202

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
			GEngine* engine = new GEngine(canvas->gc);
			engine->StartGame(game);
		}

		while (1);
	}

	UdpSocket* socket = 0;
	Label* rec_label = 0;

	void Receive()
	{
		while (1)
		{
			if (!socket)
				continue;

			IPv4Address addr;
			uint8* pkt_buf;
			int length = socket->Receive(pkt_buf, addr);
			pkt_buf[length] = 0;

			rec_label->name = (char*)pkt_buf;
		}
	}

	void RunTextEdit()
	{
		Window* wnd = WindowManager::CreateWindow("Title");
		
		//Canvas* canvas = (Canvas*)wnd->CreateControl(CONTROL_TYPE_CANVAS, "", 10, 10, 300, 300, ID_CANVAS);
		Label* lbl1 = (Label*)wnd->CreateControl(CONTROL_TYPE_LABEL, "Label", 100, 50, 300, 16, ID_LBL_1);
		Label* lbl2 = (Label*)wnd->CreateControl(CONTROL_TYPE_LABEL, "", 100, 100, 300, 16, ID_LBL_2);
		Label* lbl3 = (Label*)wnd->CreateControl(CONTROL_TYPE_LABEL, "", 100, 120, 300, 16, ID_LBL_3);
		Button* btn1 = (Button*)wnd->CreateControl(CONTROL_TYPE_BUTTON, "Button", 10, 10, 80, 20, ID_BTN_1);
		Button* btn2 = (Button*)wnd->CreateControl(CONTROL_TYPE_BUTTON, "Exit", 100, 10, 80, 20, ID_BTN_2);
		Button* btn3 = (Button*)wnd->CreateControl(CONTROL_TYPE_BUTTON, "Receive", 10, 40, 80, 20, ID_BTN_3);
		TextBox* txt1 = (TextBox*)wnd->CreateControl(CONTROL_TYPE_TEXTBOX, "", 300, 10, 200, 20, ID_TXT_1);

		IPv4Address addr;
		addr.n[0] = 0xC0;
		addr.n[1] = 0xA8;
		addr.n[2] = 0x00;
		addr.n[3] = 0x7B;

		bool listen = 0;
		if (!socket)
		{
			socket = UDP::CreateSocket(1881);
			rec_label = lbl3;
			Task::InsertThread(Task::CreateThread(Receive));
			//Receive();
		}

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
			case WM_COMMAND:
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

					socket->Send(addr, (uint8*)txt1->text.ToChar(), txt1->text.Length() + 1);
					txt1->text = "";
					break;

				case ID_BTN_2:
					WindowManager::CloseWindow(wnd);
					break;

				case ID_BTN_3:
					IPv4Address addr;
					uint8* pkt_buf;
					int length = socket->Receive(pkt_buf, addr);
					pkt_buf[length] = 0;

					lbl3->name = (char*)pkt_buf;
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