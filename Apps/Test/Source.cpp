#include <AndyOS.h>
#include <sys/msg.h>
#include "stdio.h"
#include "string.h"
#include "math.h"

void sig_handler(int signo)
{
	debug_print("SIGNAL %i\n", signo);
}

void msg_handler(int type, char* buf, int size)
{
	debug_print("MESSAGE %i %s\n", size, buf);
}

int main()
{
	set_signal(sig_handler);
	set_message(msg_handler);
	sleep(200);

	send_signal(2, 121);
	
	char* msg = "Hello";
	send_message(2, 1, msg, strlen(msg) + 1);

	while(get_ticks() != -1)
	{
		while(!get_key_down(KEY_SPACE));
		send_signal(2, 1337);
		while(get_key_down(KEY_SPACE));
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