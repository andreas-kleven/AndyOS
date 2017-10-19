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

	/*if (n == 0)
		return 1;
	
	if (n > 0)
	{
		return pow(x, n - 1);
	}
	else
	{
		return 1 / pow(x, -n);
	}*/
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


float sin(float x)
{
	float val;
	_asm
	{
		fld[x]
		fsin
		fstp[val]
	}
	return val;
}

float cos(float x)
{
	float val;
	_asm
	{
		fld[x]
		fcos
		fstp[val]
	}
	return val;
}

float tan(float x)
{
	return sin(x) / cos(x);

	float val;
	_asm
	{
		fld[x]
		fsincos
		; fdivrp st1, st0
		fstp[val]
	}
	return val;
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

int rand()
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % RAND_MAX;
}

void srand(unsigned int seed)
{
	next = seed;
}