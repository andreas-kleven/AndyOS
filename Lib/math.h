#pragma once

#define M_PI 3.14159265359
#define M_E 2.71828182845904523536

float fact(float number);
float pow(float x, int n);
float sqrt(float square);

float sin(float rad);
float cos(float rad);
float tan(float rad);

int floor(float val);
int ceil(float val);
int round(float val);

float log(float val);
float log2(float val);
float log10(float val);
float logn(float val, float n);

unsigned int rand();
void srand(unsigned int seed);


template <class T>
inline T clamp(T val, T min, T max)
{
	if (val < min)
		return min;

	if (val > max)
		return max;

	return val;
}

template <class T>
inline T abs(T val)
{
	return (val > 0) ? val : -val;
}

template <class T>
inline T min(T a, T b)
{
	if (a < b)
		return a;
	return b;
}

template <class T>
inline T max(T a, T b)
{
	if (a > b)
		return a;
	return b;
}

template <class T>
inline T min(T a, T b, T c)
{
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

template <class T>
inline T max(T a, T b, T c)
{
	return a > b ? (a > c ? a : c) : (b > c ? b : c);
}