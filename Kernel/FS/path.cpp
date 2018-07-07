#include "path.h"
#include "ctype.h"
#include "string.h"
#include "debug.h"

Path::Path()
{
	count = 0;
	parts = 0;
}

Path::Path(const char* _path)
	: Path()
{
	if (!_path)
		return;

	char* path = new char[strlen(_path) + 1];
	strcpy(path, _path);

	//Count parts
	char* str = path;

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
		parts = new char*[count];

		char* saveptr;
		char* part = strtok_r(path, "/", &saveptr);
		parts[0] = part;

		for (int i = 1; i < count; i++)
		{
			part = strtok_r(0, "/", &saveptr);
			parts[i] = part;
		}
	}
}

Path* Path::Parent() const
{
	Path* parent = new Path;
	parent->count = this->count - 1;
	parent->parts = this->parts;
	return parent;
}

bool Path::operator==(const Path& path) const
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