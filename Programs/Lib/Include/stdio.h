#pragma once
#include "stdarg.h"
#include "sys/types.h"

#define EOF         (-1)
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define O_ACCMODE	00000003
#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002
#define O_CREAT		00000100	/* not fcntl */
#define O_EXCL		00000200	/* not fcntl */
#define O_NOCTTY	00000400	/* not fcntl */
#define O_TRUNC		00001000	/* not fcntl */
#define O_APPEND	00002000
#define O_NONBLOCK	00004000
#define O_DSYNC		00010000	/* used to be O_SYNC, see below */
#define FASYNC		00020000	/* fcntl, for BSD compatibility */
#define O_DIRECT	00040000	/* direct disk access hint */
#define O_LARGEFILE	00100000
#define O_DIRECTORY	00200000	/* must be a directory */
#define O_NOFOLLOW	00400000	/* don't follow links */
#define O_NOATIME	01000000
#define O_CLOEXEC	02000000	/* set close_on_exec */

typedef signed long long fpos_t;

struct FILE
{
};

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

FILE* fopen(const char* filename, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
int fseek(FILE* stream, long int offset, int origin);
int fputc(int character, FILE* stream);
int fputs(const char* str, FILE* stream);
//int fgetc(FILE* stream);
//char* fgets(char* str, int count, FILE* stream);

int printf(const char* format, ...);
int fprintf(FILE* stream, const char* format, ...);
int sprintf(char* str, const char* format, ...);
int vprintf(const char* format, va_list vlist);
int vfprintf(FILE* stream, const char* format, va_list vlist);
int vsprintf(char* buffer, const char* format, va_list vlist);

long strtol(const char* nptr, char** endptr, int base);

int atoi(const char* str);
char* itoa(int i, unsigned base, char* buf, bool sign = 1);

float atof(const char* s);
char* ftoa(float f, unsigned base, char* buf);