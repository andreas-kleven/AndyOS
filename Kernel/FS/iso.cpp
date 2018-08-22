#include "iso.h"
#include "string.h"
#include "Lib/debug.h"

#define FILE_FLAG_HIDDEN			(1 << 0)
#define FILE_FLAG_DIRECTORY			(1 << 1)
#define FILE_FLAG_ASSOSCIATED		(1 << 2)
#define FILE_FLAG_EXTENDED			(1 << 3)
#define FILE_FLAG_EXTENDED_OWNER	(1 << 4)
#define FILE_FLAG_NOT_FINAL			(1 << 7)

ISO_FS::ISO_FS(BlockDriver* driver)
{
	this->driver = driver;

	char* buf = new char[0x1000];
	if (!driver->Read(0x10 * ISO_SECTOR_SIZE, buf, 0x1000))
		return;

	desc = (ISO_PRIMARY_DESC*)buf;

	ISO_DIRECTORY* table = &desc->rootDirectory;

	buf = new char[table->filesize_LSB];
	if (!driver->Read(table->locationLBA_LSB * ISO_SECTOR_SIZE, buf, table->filesize_LSB))
		return;

	root = (ISO_DIRECTORY*)buf;
}

int ISO_FS::Read(FILE* file, char* buf, size_t size)
{
	return driver->Read(file->node->pos, buf, size);
}

bool ISO_FS::GetFile(const Path* path, FNODE* node)
{
	ISO_DIRECTORY* dir = FindDirectory(path);

	if (dir)
	{
		node->size = dir->filesize_LSB;
		node->pos = dir->locationLBA_LSB * ISO_SECTOR_SIZE;

		if (dir->flags & FILE_FLAG_DIRECTORY)
			node->type = FILE_TYPE_REGULAR;
		else
			node->type = FILE_TYPE_DIRECTORY;

		return true;
	}

	return false;
}


ISO_DIRECTORY* ISO_FS::FindDirectory(const Path* path)
{
	ISO_DIRECTORY* start = root;
	ISO_DIRECTORY* dir = start;

	if (!dir || path->count == 0)
		return 0;

	int pi = 0;
	char* part = path->parts[pi++];

	while (part && dir && dir->length)
	{
		char name[32];
		GetName(dir, name);

		if (strcicmp(name, part) == 0)
		{
			if (dir->flags & FILE_FLAG_DIRECTORY)
			{
				if (next)
				{
					char* buffer = new char[dir->filesize_LSB];
					driver->Read(dir->locationLBA_LSB * ISO_SECTOR_SIZE, buffer, dir->filesize_LSB);
					start = (ISO_DIRECTORY*)buffer;
					dir = start;

					part = path->parts[pi++];
				}
				else
				{
					return dir;
				}
			}
			else
			{
				if (!next)
				{
					return dir;
				}
			}
		}

		dir = (ISO_DIRECTORY*)((size_t)dir + dir->length);
		if ((size_t)dir - (size_t)start > ISO_SECTOR_SIZE - sizeof(ISO_DIRECTORY))
		{
			dir = (ISO_DIRECTORY*)((size_t)start + ISO_SECTOR_SIZE);
			start = dir;
		}
	}

	return 0;
}

void ISO_FS::GetName(ISO_DIRECTORY* dir, char* buf)
{
	memcpy(buf, &dir->identifier, dir->idLength);
	buf[dir->idLength] = 0;

	int length = strlen(buf);

	if (length >= 2)
	{
		if (buf[length - 2] == ';')
			buf[length - 2] = 0;
	}

	if (length >= 3)
	{
		if (buf[length - 3] == '.')
			buf[length - 3] = 0;
	}
}