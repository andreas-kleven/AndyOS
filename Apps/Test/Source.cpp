#include <AndyOS.h>
#include <sys/msg.h>
#include "GUI.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

class MainWindow : public Window
{
public:
	MainWindow(char* title)
		: Window(title)
	{

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
	char title[256];
	vprintf(title, "Test window: %i", get_ticks());
	MainWindow wnd(title);

	while (1)
	{
		for (int i = 0; i < wnd.width * wnd.height; i++)
		{
			char r = rand() / 0xFF;
			char g = rand() / 0xFF;
			char b = rand() / 0xFF;
			wnd.framebuffer[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
		}
	}
}