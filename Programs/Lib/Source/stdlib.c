#include "stdlib.h"
#include "unistd.h"

int abs(int n)
{
	return (n > 0) ? n : -n;
}

long labs(long n)
{
	return (n > 0) ? n : -n;
}

void exit(int status)
{
    return _exit(status);
}