#include <FS/path.h>
#include <ctype.h>
#include <string.h>
#include <debug.h>

Path::Path()
{
	buf = 0;
	count = 0;
	parts = 0;
}

Path::Path(const char *_path)
	: Path()
{
	if (!_path)
		return;

	buf = strdup(_path);

	//Count parts
	char *str = buf;

	if (*str == '/')
		str++;

	if (*str)
		count++;

	while (*str)
	{
		if (*str++ == '/')
		{
			if (*str != 0 && *str != '/')
				count++;
		}
	}

	if (count > 0)
	{
		parts = new char *[count];

		char *saveptr;
		char *part = strtok_r(buf, "/", &saveptr);
		parts[0] = part;

		for (int i = 1; i < count; i++)
		{
			part = strtok_r(0, "/", &saveptr);
			parts[i] = part;
		}
	}
}

Path::~Path()
{
	for (int i = 0; i < this->count; i++)
		delete[] this->parts[i];
}

Path Path::Parent() const
{
	Path parent;
	parent.count = this->count - 1;
	parent.parts = this->parts;
	return parent;
}

const char *Path::Filename() const
{
	if (count == 0)
		return buf;

	return parts[count - 1];
}

bool Path::operator==(const Path &path) const
{
	if (this->count != path.count)
		return false;

	for (int i = 0; i < this->count; i++)
	{
		if (strcmp(this->parts[i], path.parts[i]))
		{
			return false;
		}
	}

	return true;
}