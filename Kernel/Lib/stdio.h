#pragma once
#include "stdarg.h"
#include "types.h"

#define EOF         (-1)
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

typedef signed long long fpos_t;

int sprintf(char* str, const char* format, ...);
int vsprintf(char* buffer, const char* format, va_list vlist);
int vsnprintf(char* buffer, size_t size, const char* format, va_list vlist);

long strtol(const char* nptr, char** endptr, int base);

int atoi(const char * str);
char* itoa(int i, char* buf, unsigned base, bool sign = 1);

float atof(const char* s);
char* ftoa(float f, char* buf, unsigned base);