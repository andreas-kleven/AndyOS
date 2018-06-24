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

	char* a = new char;
	while(1);

	set_signal(sig_handler);
	set_message(msg_handler);
	sleep(200);

	send_signal(2, 121);
	
	char* msg = "Hello";
	MESSAGE response = send_message(2, 1, msg, strlen(msg) + 1);
	debug_print("Received\t%ux\t%s\n", response.id, response.data);

	if (response.id == 3)
	{
		debug_print("Stop");
		while (1);
	}

	while (get_ticks() != -1)
	{
		while (!get_key_down(KEY_SPACE));
		send_signal(2, 1337);
		MESSAGE response = send_message(2, 1, msg, strlen(msg) + 1);
		debug_print("Received\t%ux\t%s\n", response.id, response.data);
		while (get_key_down(KEY_SPACE));
	}

	sleep(1000000);

	int result = 0;

	int hour = 0;
	int minute = 0;
	int second = 0;

	gettime(hour, minute, second);

	srand(hour * 100 + minute * 10 + second);

	char buf[128];

	uint32* framebuffer = new uint32[1024 * 768];

	while (1)
	{
		for (int i = 0; i < 1024 * 768; i++)
		{
			uint8 r = rand() / 0xFF;
			uint8 g = rand() / 0xFF;
			uint8 b = rand() / 0xFF;
			framebuffer[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
		}

		gettime(hour, minute, second);
		vprintf(buf, "%i:%i:%i\n", hour, minute, second);
		print(buf);

		sleep(100);
		draw(framebuffer);
	}

	while (1);
	return 2;
}