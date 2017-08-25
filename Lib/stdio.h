#pragma once
#include "stdarg.h"

char* vprintf(char* buf, const char* format, ...);
char* vsprintf(char* buf, const char* format, va_list args);
long strtol(const char* nptr, char** endptr, int base);

int atoi(const char * str);
char* itoa(int i, unsigned base, char* buf, bool sign = 1);

float atof(const char* s);
char* ftoa(float f, unsigned base, char* buf);