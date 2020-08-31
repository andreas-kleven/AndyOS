#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static char digitchars[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

int sprintf(char *str, const char *format, ...)
{
    va_list va;
    va_start(va, format);
    int ret = vsprintf(str, format, va);
    va_end(va);
    return ret;
}

int vsprintf(char *buffer, const char *format, va_list vlist)
{
    return vsnprintf(buffer, 0, format, vlist);
}

int vsnprintf(char *buffer, size_t size, const char *format, va_list vlist)
{
    if (!buffer || !format) {
        *buffer = 0;
        return -1;
    }

    char *ptr = buffer;
    char *end = 0;

    if (size)
        end = buffer + size;

    for (size_t i = 0; i <= strlen(format); i++) {
        if (end && end - ptr <= 20)
            break;

        if (format[i] == '%') {
            char specifier = format[++i];

            switch (specifier) {
            case 's': // string
            {
                char *s = va_arg(vlist, char *);

                if (end)
                    strncpy(ptr, s, end - ptr - 1);
                else
                    strcpy(ptr, s);
                break;
            }

            case 'c': // character
            {
                *ptr++ = va_arg(vlist, int);
                *ptr = 0;
                break;
            }

            case 'd': // signed int
            case 'i': {
                int num = va_arg(vlist, int);
                itoa(num, ptr, 10);
                break;
            }

            case 'u': // unsigned int
            {
                int num = va_arg(vlist, int);
                itoa(num, ptr, 10, false);
                break;
            }

            case 'o': // unsigned octal
            {
                int num = va_arg(vlist, int);
                itoa(num, ptr, 8, false);
                break;
            }

            case 'x': // unsigned hexadecimal int
            case 'X': {
                int num = va_arg(vlist, int);
                itoa(num, ptr, 16, false);
                break;
            }

            case 'f': // floating point
            case 'F': {
                double num = va_arg(vlist, double);
                ftoa(num, ptr, 10);
                break;
            }

            case 'a': // hexadecimal floating point
            case 'A': {
                double num = va_arg(vlist, double);
                ftoa(num, ptr, 16);
                break;
            }

            case 'p': {
                void *p = va_arg(vlist, void *);
                *ptr++ = '0';
                *ptr++ = 'x';
                itoa((unsigned long long)p, ptr, 16, false);
                break;
            }

            case '%': // percent
            {
                *ptr++ = '%';
                *ptr = 0;
                break;
            }

            default: // invalid specifier
            {
                *ptr++ = '%';
                *ptr++ = specifier;
                *ptr = 0;
                break;
            }
            }

            switch (specifier) {
            case 'X':
            case 'F':
            case 'A':
                stoupper(ptr);
                break;
            }

            // Move to end of string
            ptr += strlen(ptr);
        } else {
            *ptr++ = format[i];
        }
    }

    *ptr = 0;
    return strlen(buffer);
}

// Converts a string to a long
long strtol(const char *nptr, char **endptr, int base)
{
    register const unsigned char *s = (const unsigned char *)nptr;
    register unsigned long acc;
    register int c;
    register unsigned long cutoff;
    register int neg = 0, any, cutlim;

    /*
     * See strtol for comments as to the logic used.
     */
    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;
    cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
    cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) {
        acc = ULONG_MAX;
    } else if (neg)
        acc = -acc;
    if (endptr != 0)
        *endptr = (char *)(any ? (char *)s - 1 : nptr);
    return (acc);
}

// Convert string to int
int atoi(const char *str)
{

    return (int)strtol(str, 0, 10);
}

// Converts int to string
char *itoa(int i, char *buf, unsigned base, bool sign)
{
    if (base > 16)
        return buf;

    if (i < 0) {
        if (sign) {
            *buf++ = '-';
            i *= -1;
        }
    }

    char tbuf[32];

    int pos = 0;
    int opos = 0;
    int top = 0;

    if (i == 0 || base > 16) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    while (i != 0) {
        tbuf[pos] = digitchars[i % base];
        pos++;
        i /= base;
    }
    top = pos--;
    for (opos = 0; opos < top; pos--, opos++) {
        buf[opos] = tbuf[pos];
    }
    buf[opos] = 0;

    return buf;
}

// Converts string to float
float atof(const char *s)
{
    float rez = 0, fact = 1;
    if (*s == '-') {
        s++;
        fact = -1;
    }

    for (int point_seen = 0; *s; s++) {
        if (*s == '.') {
            point_seen = 1;
            continue;
        }

        int d = *s - '0';
        if (d >= 0 && d <= 9) {
            if (point_seen)
                fact /= 10.0f;
            rez = rez * 10.0f + (float)d;
        }
    }
    return rez * fact;
}

#define PRECISION 0.00001
// Converts float to string
char *ftoa(float f, char *buf, unsigned base)
{
    char *_buf = buf;

    if (f < 0) {
        *(buf++) = '-';
        f *= -1;
    }

    if (isnan(f)) {
        strcpy(buf, "nan");
    } else if (isinf(f)) {
        strcpy(buf, "inf");
    } else if (abs(f) < PRECISION) {
        strcpy(buf, "0");
    } else {
        float logbase = logn(f, base);

        int m = (int)logbase;
        int digit;

        if (m < 1)
            m = 0;

        if (f <= 0 + PRECISION && m == 0) {
            strcpy(buf, "0");
        } else {
            while (f > 0 + PRECISION || m >= 0) {
                if (abs(f) < PRECISION) {
                    *(buf++) = '0';
                } else {
                    float weight = pow(base, m);
                    digit = floor(f / weight);

                    f -= (digit * weight);
                    *(buf++) = digitchars[digit];
                }

                if (m == 0)
                    *(buf++) = '.';
                m--;
            }

            if (*(buf - 1) == '.')
                *(--buf) = '\0';
            else
                *(buf) = '\0';
        }
    }
    return _buf;
}
