#pragma once

unsigned int strcmp(const char* str1, const char* str2);
char* strcpy(char *dest, const char *src);
unsigned int strlen(const char* str);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, int n);

//extern "C" void* __cdecl memcpy(void* dest, void* src, unsigned int n);
void* memcpy(void* dest, void* src, unsigned int n);
void memcpy_fast_128(void* dest, void* src, unsigned int count);

extern "C" void* __cdecl memset(void*, int, int);
#pragma intrinsic(memset)
//void* memset(void *dest, unsigned char val, unsigned int n);
void* memset16(void *dest, unsigned short val, unsigned int n);
void* memset32(void *dest, unsigned int val, unsigned int n);
void* memset_fast_128(void* dest, unsigned int val, unsigned int n);

int memcmp(const void* a, const void* b, unsigned int n);