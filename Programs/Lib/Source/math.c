#include "math.h"
#include "stdlib.h"

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

float sqrt(float val)
{
	asm("fsqrt" : "+t" (val));
    return val;
}

float sin(float val)
{
	asm("fsin" : "+t" (val));
    return val;
}

float cos(float val)
{
	asm("fcos" : "+t" (val));
    return val;
}

float tan(float val)
{
	return sin(val) / cos(val);
}

float asin(float val)
{
	return atan2(val, sqrt((1.0 + val) * (1.0 - val)));
}

float acos(float val)
{
	return atan2(sqrt((1.0 + val) * (1.0 - val)), val);
}

float atan(float val)
{
	return atan2(val, 1);
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
	if (fabs(z) < 1.0f)
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
	//https://sites.google.com/site/akohlmey/news-and-announcements/twolinesofcode40faster
	double result;
    __asm__ __volatile__ ("fldln2\nfxch\nfyl2x" 
                      : "=t" (result) 
                      : "0" (val) : "st(1)");
    return result;
}

float log2(float val)
{
	return log(val) / log(2);
}

float log10(float val)
{
	return log(val) / log(10);
}

float logn(float val, float n)
{
	return log(val) / log(n);
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