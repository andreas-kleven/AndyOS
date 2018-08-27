#pragma once
#include "size_t.h"

#ifdef __cplusplus
extern "C" {
#endif

int strcmp(const char* str1, const char* str2);
int stricmp(char const *a, char const *b);
char* strcpy(char *dest, const char *src);
char* strncpy(char *dest, const char *src, int length);
size_t strlen(const char* str);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, int n);

void stoupper(char* s);
void stolower(char* s);

void* memcpy(void* dest, const void* src, size_t n);
int memcmp(const void* a, const void* b, size_t n);

void* memset(void* dest, int val, int n);
void* memset16(void* dest, unsigned short val, size_t n);
void* memset32(void* dest, unsigned int val, size_t n);

char* search(const char* str, const char* delim);
char* strtok_r(char* str, const char* delim, char** saveptr);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class String
{
public:
	String()
	{
		buffer = new char[0x1000];
		buffer[0] = 0;
	}

	String(const char* str)
	{
		int length = strlen(str);
		buffer = new char[length + 1];
		memcpy(buffer, str, length + 1);
	}

	~String()
	{
		delete[] buffer;
	}

	inline int Length() const { return strlen(buffer); }

	inline char* ToChar() const
	{
		int len = Length();
		char* c = new char[len + 1];
		strcpy(c, buffer);
		c[len] = 0;
		return c;
	}

	bool Contains(String str) const
	{
		char* a = buffer;

		while (*a)
		{
			char* temp = a++;
			char* b = str.buffer;

			while (*temp++ == *b++)
			{
				if (!*b)
					return 1;
			}
		}

		return 0;
	}

	String Remove(int index) const
	{
		int length = index + 1;

		if (length > Length() || length < 0)
			return "";

		char newStr[length];
		strncpy(newStr, buffer, length);
		return String(newStr);
	}

	/*void String::Split(List<String>& out, char delim)
	{
		char* start = buffer;
		char* end = buffer;

		while (*end)
		{
			while (*end != delim && *end++);

			while (*end == delim)
				*end++ = 0;

			out.Add(start);
			start = end;
		}
	}*/

	String& operator+=(const String& str)
	{
		strcat(buffer, str.buffer);
		return *this;
	}

	/*String& operator+=(const char*& str)
	{
	strcat(buffer, str);
	return *this;
	}

	String& operator+=(char*& str)
	{
	strcat(buffer, str);
	return *this;
	}*/

	String& operator+=(const char c)
	{
		int len = Length();
		buffer[len++] = c;
		buffer[len] = 0;
		return *this;
	}

	String& operator+=(const char str[])
	{
		strcat(buffer, str);
		return *this;
	}

	String& operator+(const char*& str)
	{
		String* ns = new String(buffer);
		*ns += str;
		return *ns;
	}

	String& operator+(const String& str)
	{
		String* ns = new String(buffer);
		*ns += str;
		return *ns;
	}

	String& operator=(const char*& str)
	{
		strcpy(buffer, str);
		return *this;
	}

	String& operator=(const String& str)
	{
		strcpy(buffer, str.buffer);
		return *this;
	}

	bool operator==(const String& str)
	{
		return !strcmp(buffer, str.buffer);
	}

private:
	char* buffer;
};

#endif