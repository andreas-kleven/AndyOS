#include "path.h"
#include "ctype.h"
#include "string.h"

char Path::GetDriveLetter(char* path)
{
	char letter = *path++;

	if (!isascii(letter))
		return 0;

	if (*path++ != ':')
		return 0;

	if (*path++ != '/')
		return 0;

	return letter;
}
