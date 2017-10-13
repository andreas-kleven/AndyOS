#pragma once
#include "../Lib/string.h"
#include "List.h"
#include "System.h"

namespace std
{
	class String
	{
	public:
		String()
		{
			buffer = new char[0x1000];
			buffer[0] = 0;
		}

		String(char* str)
		{
			int length = strlen(str);
			buffer = new char[length];
			buffer[0] = 0;
			memcpy(buffer, str, length);
		}

		~String()
		{
			delete[] buffer;
		}

		inline int Length() { return strlen(buffer) - 1; }

		inline char* ToChar()
		{
			int len = Length();
			char* c = new char[len + 1];
			strcpy(c, buffer);
			c[len] = 0;
			return c;
		}

		bool Contains(String str)
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

		void String::Split(List<String>& out, char delim);

		String& operator+=(String& str)
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

		String& operator+=(const char str[])
		{
			strcat(buffer, str);
			return *this;
		}

		String& operator+=(const char c)
		{
			int len = Length();
			buffer[len++] = c;
			buffer[len] = 0;
			return *this;
		}

		String& operator+(String& str)
		{
			String* ns = new String(buffer);
			*ns += str;
			return *ns;
		}

		String& operator+(const char*& str)
		{
			String* ns = new String(buffer);
			*ns += str;
			return *ns;
		}

		String& operator+(char*& str)
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
}