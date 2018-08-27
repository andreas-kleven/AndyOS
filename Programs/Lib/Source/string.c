#include "string.h"
#include "sys/types.h"
#include "ctype.h"

int strcmp(const char* str1, const char* str2)
{
	if (!str1 || !str2)
		return str1 != str2;

	int res = 0;
	while (!(res = *(unsigned char*)str1 - *(unsigned char*)str2) && *str2)
		++str1, ++str2;

	if (res < 0)
		res = -1;
	if (res > 0)
		res = 1;

	return res;
}

int stricmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}

char* strcpy(char* dest, const char* src)
{
	char *s1_p = dest;
	while ((*dest++ = *src++) != 0);
	return s1_p;
}

char* strncpy(char* dest, const char* src, int length)
{
	char *s1_p = dest;
	while (length && (*dest++ = *src++)) length--;

	*dest = 0;
	while (length--) *dest++ = 0;
	return s1_p;
}

size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len++]);
	return len - 1;
}

char* strcat(char* dest, const char* src)
{
	char* rdest = dest;

	while (*dest)
		dest++;

	while ((*dest++ = *src++) != 0);
	return rdest;
}

char* strncat(char * dest, const char* src, int n)
{
	char *rdest = dest;

	while (*++dest);

	while (n-- && (*dest++ = *src++));
	*rdest = 0;
	return rdest;
}

void stoupper(char* s)
{
    for(; *s; s++)
        if(('a' <= *s) && (*s <= 'z'))
            *s = 'A' + (*s - 'a');
}

void stolower(char* s)
{
    for(; *s; s++)
        if(('A' <= *s) && (*s <= 'Z'))
            *s = 'a' + (*s - 'A');
}

void* memcpy(void* dest, const void* src, size_t n)
{
	char* a = (char*)src;
	char* b = (char*)dest;

	while (n--)
		*b++ = *a++;

	return dest;
}

void* memset(void* dest, int val, int n)
{
	unsigned char *temp = (unsigned char *)dest;
	for (; n != 0; n--, temp[n] = val);
	return dest;
}

void* memset16(void *dest, unsigned short val, size_t n)
{
	unsigned short *temp = (unsigned short *)dest;
	for (; n != 0; n--, temp[n] = val);
	return dest;
}

void* memset32(void *dest, unsigned int val, size_t n)
{
	unsigned int* temp = (unsigned int *)dest;
	for (; n != 0; n--, temp[n] = val);
	return dest;
}

int memcmp(const void* a, const void* b, size_t n)
{
	const unsigned char* cs = (const unsigned char*)a;
	const unsigned char* ct = (const unsigned char*)b;

	for (int i = 0; i < n; i++, cs++, ct++)
	{
		if (*cs < *ct)
			return -1;
		else if (*cs > *ct)
			return 1;
	}
	return 0;
}

//void* memset(void* dest, int a, int b)
//{
//	//return nullptr;
//	return memset(dest, a, b);
//}

char* search(const char* str, const char* delim)
{
	char c = *str++;

	while (c)
	{
		const char* del = delim;
		char d = *del++;

		while (d)
		{
			if (c == d)
				return (char*)str;

			d = *del++;
		}

		c = *str++;
	}

	return 0;
}

//Splits string
char* strtok_r(char* str, const char* delim, char** saveptr)
{
	if (!str)
		str = *saveptr;

	*saveptr = 0;

	if (!str)
		return 0;

	//Remove delimiters characters at beginning
	const char* d;

loop:
	d = delim;

	while (*d++ == *str)
	{
		str++;
		goto loop;
	}

	if (*str == 0)
		return 0;

	char* end = search(str, delim);

	if (end)
	{
		*(end - 1) = 0;
		*saveptr = end;
		return str;
	}
	else
	{
		*saveptr = 0;
		return str;
	}

	return 0;
}