#include "string.h"
#include "debug.h"

namespace std
{
	void String::Split(List<String>& out, char delim)
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
	}
}