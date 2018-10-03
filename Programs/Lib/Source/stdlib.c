#include "stdlib.h"
#include "unistd.h"
#include "syscall.h"

int abs(int n)
{
	return (n > 0) ? n : -n;
}

long labs(long n)
{
	return (n > 0) ? n : -n;
}

float fabs(float n)
{
	return (n > 0) ? n : -n;
}

char* getenv(const char* name)
{
	return 0;
}

int atoi(const char * str) {

	return (int)strtol(str, 0, 10);
}

float atof(const char* s)
{
	float rez = 0, fact = 1;
	if (*s == '-')
	{
		s++;
		fact = -1;
	}

	for (int point_seen = 0; *s; s++)
	{
		if (*s == '.') {
			point_seen = 1;
			continue;
		}

		int d = *s - '0';
		if (d >= 0 && d <= 9) {
			if (point_seen) fact /= 10.0f;
			rez = rez * 10.0f + (float)d;
		}
	}
	return rez * fact;
}

void* malloc(size_t size)
{
	return (uint32*)syscall1(SYSCALL_ALLOC, size / 4096);
}

void free(void* p)
{
	syscall2(SYSCALL_ALLOC, (int)p, 1);
}

void abort()
{
	exit(1);
}

void exit(int status)
{
    return _exit(status);
}