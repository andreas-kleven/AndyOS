#include <AndyOS.h>
#include <sys/msg.h>
#include "GUI.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

using namespace gui;

void btnClick();

class MainWindow : public Window
{
public:
	Label* label;
	Button* button;

	MainWindow(char* title)
		: Window(title)
	{
		label = new Label("Label");
		button = new Button("A button");

		button->bounds = Rect(100, 200, 80, 20);
		button->OnClick = btnClick;

		AddChild(label);
		AddChild(button);
	}
};

MainWindow* wnd;

int clicks = 0;
void btnClick()
{
	char buf[100];
	vprintf(buf, "Clicked %i", ++clicks);
	wnd->label->text = buf;
}

int main()
{
	Drawing::Init();

	char title[256];
	vprintf(title, "Test window: %i", get_ticks());
	wnd = new MainWindow(title);

	while (1)
	{
		wnd->Paint();
		sleep(100);
		continue;

		for (int i = 0; i < wnd->width * wnd->height; i++)
		{
			char r = rand() / 0xFF;
			char g = rand() / 0xFF;
			char b = rand() / 0xFF;
			wnd->gc.framebuffer[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
		}
	}
}