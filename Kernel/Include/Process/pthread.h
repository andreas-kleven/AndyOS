#pragma once

#define __SIZEOF_PTHREAD_ATTR_T 36
//#define __SIZEOF_PTHREAD_ATTR_T 32

typedef unsigned long int pthread_t;

typedef union
{
    char size[__SIZEOF_PTHREAD_ATTR_T];
    long int align;
} pthread_attr_t;
