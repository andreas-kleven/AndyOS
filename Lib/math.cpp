#include "math.h"

float fact(float number)
{
	if (number == 0) return 1;
	return number * fact(number - 1);
}

float pow(float x, int n)
{
	float y = 1.0;
	for (int i = 0; i < n; i++)
		y *= x;
	for (int i = 0; i > n; i--)
		y /= x;
	return y;
}

float sqrt(float square)
{
	float root;
	_asm
	{
		fld[square]
		fsqrt
		fstp[root]
	}
	return root;
}


float sin(float rad)
{
	float val;
	_asm
	{
		fld[rad]
		fsin
		fstp[val]
	}
	return val;
}

float cos(float rad)
{
	float val;
	_asm
	{
		fld[rad]
		fcos
		fstp[val]
	}
	return val;
}

float tan(float rad)
{
	return sin(rad) / cos(rad);

	float val;
	_asm
	{
		fld[rad]
		fsincos
		; fdivrp st1, st0
		fstp[val]
	}
	return val;
}


int floor(float val)
{
	if (val > 0)
		return (int)val;

	return (int)val - 1;
}

int ceil(float val)
{
	if (val < 0)
		return (int)val;

	return (int)val + 1;
}

int round(float val)
{
	return (int)(val + 0.5);
}


float log(float val)
{
	float ret;
	_asm
	{
		fld1
		fld[val]
		fyl2x

		fldl2e
		fdiv
		fstp[ret]
	}
	return ret;
}

float log2(float val)
{
	float ret;
	_asm
	{
		fld1
		fld[val]
		fyl2x
		fstp[ret]
	}
	return ret;
}

float log10(float val)
{
	float ret;
	_asm
	{
		fld1
		fld[val]
		fyl2x

		fldl2t
		fdiv
		fstp[ret]
	}
	return ret;
}

float logn(float val, float n)
{
	float ret;
	_asm
	{
		fld1
		fld[val]
		fyl2x

		fld1
		fld[n]
		fyl2x

		fdiv
		fstp[ret]

		fdecstp
		fdecstp
	}
	return ret;
}


#define RAND_MAX 32768
static unsigned long int next = 1;

unsigned int rand()
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % RAND_MAX;
}

void srand(unsigned int seed)
{
	next = seed;
}