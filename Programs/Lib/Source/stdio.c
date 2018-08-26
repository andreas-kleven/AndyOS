#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "limits.h"
#include "math.h"
#include "unistd.h"

#define FILE_TABLE_SIZE 256

FILE file_table[FILE_TABLE_SIZE];

FILE* stdin = &file_table[0];
FILE* stdout = &file_table[1];
FILE* stderr = &file_table[2];

static char digitchars[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

int get_fd(FILE* file)
{
	int fd = ((unsigned long)file - (unsigned long)&file_table[0]) / sizeof(FILE);

	if (fd < 0 || fd >= FILE_TABLE_SIZE)
		return -1;

	return fd;
}

FILE* fopen(const char* filename, const char* mode)
{
	int fd = open(filename, O_RDWR);

	if (fd < 0 || fd >= FILE_TABLE_SIZE)
		return 0;

	return &file_table[fd];
}

int fclose(FILE* stream)
{
	int fd = get_fd(stream);
	return close(fd);
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	int fd = get_fd(stream);
	return read(fd, (char*)ptr, size*nmemb);
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	int fd = get_fd(stream);
	return write(fd, (const char*)ptr, size*nmemb);
}

int fseek(FILE* stream, long int offset, int origin)
{
	int fd = get_fd(stream);
	return seek(fd, offset, origin);
}

int fputc(int character, FILE* stream)
{
	return fwrite(&character, 1, 1, stream);
}

int fputs(const char* str, FILE* stream)
{
	return fwrite(str, 1, strlen(str), stream);
}

int printf(const char* format, ...)
{
	va_list	va;
	va_start(va, format);
	int ret = vprintf(format, va);
	va_end(va);
	return ret;
}

int fprintf(FILE* stream, const char* format, ...)
{
	va_list	va;
	va_start(va, format);
	int ret = vfprintf(stream, format, va);
	va_end(va);
	return ret;
}

int sprintf(char* str, const char* format, ...)
{
	va_list	va;
	va_start(va, format);
	int ret = vsprintf(str, format, va);
	va_end(va);
	return ret;
}

int vprintf(const char* format, va_list vlist)
{
	return vfprintf(stdout, format, vlist);
}

int vfprintf(FILE* stream, const char* format, va_list vlist)
{
	char buffer[512];
	int ret = vsprintf(buffer, format, vlist);

	if (ret == -1)
		return -1;

	fwrite(buffer, 1, ret, stream);
	return ret;
}

int vsprintf(char* buffer, const char* format, va_list vlist)
{
	if (!buffer || !format)
	{
		*buffer = 0;
		return -1;
	}

	char* ptr = buffer;

	for (int i = 0; i <= strlen(format); i++)
	{
		if (format[i] == '%')
		{
			char specifier = format[++i];

			switch (specifier)
			{
				case 's':	//string
				{
					char* s = va_arg(vlist, char*);
					strcpy(ptr, s);
					break;
				}

				case 'c':	//character
				{
					*ptr++ = va_arg(vlist, char);
					*ptr = 0;
					break;
				}

				case 'd':	//signed int
				case 'i':
				{
					int num = va_arg(vlist, int);
					itoa(num, 10, ptr);
					break;
				}

				case 'u':	//unsigned int
				{
					int num = va_arg(vlist, int);
					itoa(num, 10, ptr);
					break;
				}

				case 'o':	//unsigned octal
				{
					int num = va_arg(vlist, int);
					itoa(num, 8, ptr);
					break;
				}

				case 'x':	//unsigned hexadecimal int
				case 'X':
				{
					int num = va_arg(vlist, int);
					itoa(num, 16, ptr);
					break;
				}

				case 'f':	//floating point
				case 'F':
				{
					double num = va_arg(vlist, double);
					ftoa(num, 10, ptr);
					break;
				}

				case 'a':	//hexadecimal floating point
				case 'A':
				{
					double num = va_arg(vlist, double);
					ftoa(num, 16, ptr);
					break;
				}

				case 'p':
				{
					void* p = va_arg(vlist, void*);
					*ptr++ = '0';
					*ptr++ = 'x';
					itoa((size_t)p, 16, ptr);
					break;
				}

				case '%':	//percent
				{
					*ptr++= '%';
					*ptr = 0;
					break;
				}

				default:	//invalid specifier
				{
					*ptr++ = '%';
					*ptr++ = specifier;
					*ptr = 0;
					break;
				}
			}

			switch (specifier)
			{
				case 'X':
				case 'F':
				case 'A':
					stoupper(ptr);
					break;
			}

			//Move to end of string
			ptr += strlen(ptr);
		}
		else
		{
			*ptr++ = format[i];
		}
	}

	*ptr = 0;
	return strlen(buffer);
}

//Converts a string to a long
long strtol(const char* nptr, char** endptr, int base)
{
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	/*
	* Skip white space and pick up leading +/- sign if any.
	* If base is 0, allow 0x for hex and 0 for octal, else
	* assume decimal; if base is already 16, allow 0x.
	*/
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	}
	else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
		c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	else if ((base == 0 || base == 2) &&
		c == '0' && (*s == 'b' || *s == 'B')) {
		c = s[1];
		s += 2;
		base = 2;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	* Compute the cutoff value between legal numbers and illegal
	* numbers.  That is the largest legal value, divided by the
	* base.  An input number that is greater than this value, if
	* followed by a legal input character, is too big.  One that
	* is equal to this value may be valid or not; the limit
	* between valid and invalid numbers is then based on the last
	* digit.  For instance, if the range for longs is
	* [-2147483648..2147483647] and the input base is 10,
	* cutoff will be set to 214748364 and cutlim to either
	* 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	* a value > 214748364, or equal but the next digit is > 7 (or 8),
	* the number is too big, and we will return a range error.
	*
	* Set any if any `digits' consumed; make it negative to indicate
	* overflow.
	*/
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
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
		acc = neg ? LONG_MIN : LONG_MAX;
		//		errno = ERANGE;
	}
	else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *)(any ? s - 1 : nptr);
	return (acc);
}

//Convert string to int
int atoi(const char * str) {

	return (int)strtol(str, 0, 10);
}

//Converts int to string
char* itoa(int i, unsigned base, char* buf)
{
	if (base > 16)
		return buf;

	if (i < 0)
	{
		if (base == 10)
		{
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


//Converts string to float
float atof(const char* s)
{
	float rez = 0, fact = 1;
	if (*s == '-')
	{
		s++;
		fact = -1;
	}

	for (int point_seen = 0; *s; s++)
	{
		if (*s == '.') {
			point_seen = 1;
			continue;
		}

		int d = *s - '0';
		if (d >= 0 && d <= 9) {
			if (point_seen) fact /= 10.0f;
			rez = rez * 10.0f + (float)d;
		}
	}
	return rez * fact;
}

#define PRECISION 0.00001
//Converts float to string
char* ftoa(float f, unsigned base, char* buf)
{
	char* _buf = buf;

	if (f < 0)
	{
		*(buf++) = '-';
		f *= -1;
	}

	if (isnan(f))
	{
		strcpy(buf, "nan");
	}
	else if (isinf(f))
	{
		strcpy(buf, "inf");
	}
	else if (abs(f) < PRECISION)
	{
		strcpy(buf, "0");
	}
	else
	{
		float logbase = logn(f, base);

		int m = (int)logbase;
		int digit;

		if (m < 1)
			m = 0;

		if (f <= 0 + PRECISION && m == 0)
		{
			strcpy(buf, "0");
		}
		else
		{
			while (abs(f) < PRECISION || m >= 0)
			{
				if (abs(f) < PRECISION)
				{
					//Todo: fix this
					*(buf++) = '0';
					break;
				}
				else
				{
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