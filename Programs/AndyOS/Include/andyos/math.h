#pragma once

inline float frand();

extern "C++"
{
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

	template <class T>
	inline T copysign(T a, T b)
	{
		return (a < 0) == (b < 0) ? a : -a;
	}
}
