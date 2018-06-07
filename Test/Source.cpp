#include "AndyOS.h"
#include "stdio.h"

int main()
{
	int result = 0;

	int hour = 0;
	int minute = 0;
	int second = 0;

	char buf[128];

	while (1)
	{
		gettime(hour, minute, second);
		vprintf(buf, "%i:%i:%i\n", hour, minute, second);
		print(buf);
	}

	while (1);
	return 2;
}