#include "../Kernel/Lib/stdio.h"

int main()
{
	int result = 0;

	/*//Get time
	int hour = 0;
	int minute = 0;
	int second = 0;

	_asm
	{
		mov eax, 3
		lea ebx, hour
		lea ecx, minute
		lea edx, second
		int 0x80
		add esp, 4

		mov[result], eax
	}*/

	//Print
	//char text[256];
	//vprintf(text, "%i:%i:%i\n", hour, minute, second);

	const char* text = "Hello from user mode\n";

	_asm
	{
		mov eax, 2
		mov ebx, text
		int 0x80
		add esp, 4

		mov[result], eax
	}

	while (1);
	return 2;
}