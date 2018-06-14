#include "math.h"
#include "definitions.h"

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

	asm("fld (%1)\n"
		"fsqrt\n"
		"fstp (%0)"
		: "=r" (root) : "r" (square));

	return root;
}


float sin(float x)
{
	float val;

	asm("fld (%1)\n"
		"fsin\n"
		"fstp (%0)"
		: "=r" (val) : "r" (x));

	return val;
}

float cos(float x)
{
	float val;

	asm("fld (%1)\n"
		"fcos\n"
		"fstp (%0)"
		: "=r" (val) : "r" (x));

	return val;
}

float tan(float x)
{
	return sin(x) / cos(x);
}

float asin(float x)
{
	return atan2(x, sqrt((1.0 + x) * (1.0 - x)));
}

float acos(float x)
{
	return atan2(sqrt((1.0 + x) * (1.0 - x)), x);
}

float atan(float x)
{
	return atan2(x, 1);
}

float atan2(float y, float x)
{
	if (x == 0.0f)
	{
		if (y > 0.0f) return M_PI;
		if (y == 0.0f) return 0.0f;
		return -M_PI_2;
	}
	float atan;
	float z = y / x;
	if (abs(z) < 1.0f)
	{
		atan = z / (1.0f + 0.28f*z*z);
		if (x < 0.0f)
		{
			if (y < 0.0f) return atan - M_PI;
			return atan + M_PI;
		}
	}
	else
	{
		atan = M_PI_2 - z / (z*z + 0.28f);
		if (y < 0.0f) return atan - M_PI;
	}
	return atan;
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

	asm("fld1\n"
		"fld (%1)\n"
		"fyl2x\n"

		"fldl2e\n"
		"fdivp\n"
		"fstp (%0)"
		: "=r" (ret) : "r" (val));

	return ret;
}

float log2(float val)
{
	float ret;

	asm("fld1\n"
		"fld (%1)\n"
		"fyl2x\n"
		"fstp (%0)"
		: "=r" (ret) : "r" (val));

	return ret;
}

float log10(float val)
{
	float ret;

	asm("fld1\n"
		"fld (%1)\n"
		"fyl2x\n"

		"fldl2t\n"
		"fdivp\n"
		"fstp (%0)"
		: "=r" (ret) : "r" (val));

	return ret;
}

float logn(float val, float n)
{
	float ret;

	asm("fld1\n"
		"fld (%1)\n"
		"fyl2x\n"

		"fld1\n"
		"fld (%2)\n"
		"fyl2x\n"

		"fdivp\n"
		"fstp (%0)\n"

		"fdecstp\n"
		"fdecstp\n"
		: "=r" (ret) : "r" (val), "r" (n));

	return ret;
}

static unsigned long int next = 1;

unsigned int rand()
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % RAND_MAX;
}

float frand()
{
	return (float)rand() / RAND_MAX;
}

void srand(unsigned int seed)
{
	next = seed;
}