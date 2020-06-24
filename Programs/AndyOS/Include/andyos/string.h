#pragma once
#include <string.h>

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
        strcpy(buffer, str);
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
