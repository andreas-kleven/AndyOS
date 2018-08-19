#pragma once
#include "stdarg.h"
#include "definitions.h"

#define EOF         (-1)
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

char* vprintf(char* buf, const char* format, ...);
char* vsprintf(char* buf, const char* format, va_list args);
long strtol(const char* nptr, char** endptr, int base);

int atoi(const char * str);
char* itoa(int i, unsigned base, char* buf, bool sign = 1);

float atof(const char* s);
char* ftoa(float f, unsigned base, char* buf);