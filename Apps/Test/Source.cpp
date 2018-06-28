#include <AndyOS.h>
#include <sys/msg.h>
#include "GUI.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

using namespace gui;

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

		AddChild(label);
		AddChild(button);
	}
};

void sig_handler(int signo)
{
	debug_print("SIGNAL %i\n", signo);
}

MESSAGE msg_handler(MESSAGE msg)
{
	debug_print("MESSAGE %i %s\n", msg.size, msg.data);
	char* str = "Response";
	return MESSAGE(msg.type, str, strlen(str) + 1);
}

int main()
{
	Drawing::Init();

	char title[256];
	vprintf(title, "Test window: %i", get_ticks());
	MainWindow wnd(title);

	while (1)
	{
		wnd.Paint();
		sleep(100);
		continue;

		for (int i = 0; i < wnd.width * wnd.height; i++)
		{
			char r = rand() / 0xFF;
			char g = rand() / 0xFF;
			char b = rand() / 0xFF;
			wnd.gc.framebuffer[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
		}
	}
}