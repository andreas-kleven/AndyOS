#pragma once

int strcmp(const char* str1, const char* str2);
int strcicmp(char const *a, char const *b);
char* strcpy(char *dest, const char *src);
char* strncpy(char *dest, const char *src, int length);
unsigned int strlen(const char* str);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, int n);

void stoupper(char* s);
void stolower(char* s);

void* memcpy(void* dest, void* src, unsigned int n);
int memcmp(const void* a, const void* b, unsigned int n);

void* memset(void* dest, int val, int n);
void* memset16(void* dest, unsigned short val, unsigned int n);
void* memset32(void* dest, unsigned int val, unsigned int n);

char* search(const char* str, const char* delim);
char* strtok_r(const char* str, const char* delim, char** saveptr);