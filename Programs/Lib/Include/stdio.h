#pragma once
#include "stdarg.h"
#include "definitions.h"

#define EOF         (-1)
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

struct FILE
{
    unsigned int fd;
};

FILE* fopen(const char* filename, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
int fseek(FILE* stream, long int offset, int origin);
//int fputc(unsigned char c, FILE* stream);
//int fputs(const char* str, FILE* stream);
//int fgetc(FILE* stream);
//char* fgets(char* str, int count, FILE* stream);

char* vprintf(char* buf, const char* format, ...);
char* vsprintf(char* buf, const char* format, va_list args);
long strtol(const char* nptr, char** endptr, int base);

int atoi(const char * str);
char* itoa(int i, unsigned base, char* buf, bool sign = 1);

float atof(const char* s);
char* ftoa(float f, unsigned base, char* buf);