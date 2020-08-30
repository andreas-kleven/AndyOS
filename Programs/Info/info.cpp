#include <AndyOS.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    printf("Ticks: %i\n", get_ticks());
    return 0;
}
