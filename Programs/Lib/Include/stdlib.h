#pragma once
#include "stddef.h"

#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0
#define RAND_MAX        32768

#ifdef __cplusplus
extern "C" {
#endif

int abs(int n);
long labs(long n);

void exit(int status);

#ifdef __cplusplus
}
#endif