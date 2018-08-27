#pragma once
#include "stddef.h"
#include "size_t.h"

#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0
#define RAND_MAX        32768

#ifdef __cplusplus
extern "C" {
#endif

int abs(int n);
long labs(long n);
char* getenv(const char* name);
int atoi(const char* str);
float atof(const char* s);

void* malloc(size_t size);
void free(void* p);

void abort();
void exit(int status);

int atexit(void (*fcm)(void));

#ifdef __cplusplus
}
#endif