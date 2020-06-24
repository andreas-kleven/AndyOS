#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <AndyOS.h>

int main()
{
    printf("Ticks: %i\n", get_ticks());
	return 0;
}
