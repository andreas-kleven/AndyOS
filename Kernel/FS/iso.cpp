#include "iso.h"
#include "string.h"
#include "Lib/debug.h"

#define FILE_FLAG_HIDDEN			(1 << 0)
#define FILE_FLAG_DIRECTORY			(1 << 1)
#define FILE_FLAG_ASSOSCIATED		(1 << 2)
#define FILE_FLAG_EXTENDED			(1 << 3)
#define FILE_FLAG_EXTENDED_OWNER	(1 << 4)
#define FILE_FLAG_NOT_FINAL			(1 << 7)

ISO_FS::ISO_FS(BlockDevice* dev)
	: IFileSystem(dev)
{
	char* buf;
	if (!device->Read(0x10 * ISO_SECTOR_SIZE, buf, 0x1000))
		return;

	desc = (ISO_PRIMARY_DESC*)buf;

	ISO_DIRECTORY* table = &desc->rootDirectory;

	if (!device->Read(table->locationLBA_LSB * ISO_SECTOR_SIZE, buf, table->filesize_LSB))
		return;

	root = (ISO_DIRECTORY*)buf;
}

ISO_DIRECTORY* ISO_FS::FindDirectory(char* path, bool isDir)
{
	ISO_DIRECTORY* start = root;
	ISO_DIRECTORY* dir = start;

	if (!dir)
		return 0;

	char* saveptr;
	char* part = strtok_r(path, "/", &saveptr);
	char* next = strtok_r(0, "/", &saveptr);

	if (!part)
		return root;

	char* buffer;

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
					device->Read(dir->locationLBA_LSB * ISO_SECTOR_SIZE, buffer, dir->filesize_LSB);
					start = (ISO_DIRECTORY*)buffer;
					dir = start;

					part = next;
					next = strtok_r(0, "/", &saveptr);
				}
				else if (isDir)
				{
					return dir;
				}
			}
			else
			{
				if (!next && !isDir)
				{
					return dir;
				}
			}
		}

		dir = (ISO_DIRECTORY*)((int)dir + dir->length);
		if ((int)dir - (int)start > ISO_SECTOR_SIZE - sizeof(ISO_DIRECTORY))
		{
			dir = (ISO_DIRECTORY*)((int)start + ISO_SECTOR_SIZE);
			start = dir;
		}
	}

	return 0;
}

bool ISO_FS::GetDirectory(DIRECTORY_INFO* parent, char* path, DIRECTORY_INFO* dir)
{
	ISO_DIRECTORY* iso_dir = FindDirectory(path, true);

	if (!iso_dir)
		return 0;

	return ParseDirectory(iso_dir, path, dir);
}

bool ISO_FS::GetFile(DIRECTORY_INFO* dir, char* path, FILE_INFO* file)
{
	ISO_DIRECTORY* iso_dir = FindDirectory(path, false);

	if (!iso_dir)
		return 0;

	return ParseFile(iso_dir, path, file);
}

bool ISO_FS::ReadFile(FILE_INFO* file, char*& buffer)
{
	if (!device->Read(file->location, buffer, file->size))
		return 0;

	return 1;
}

bool ISO_FS::WriteFile(FILE_INFO* file, void* data, uint32 length)
{
	return 0;
}

bool ISO_FS::Count(char* path, bool recursive, int& file_count, int& dir_count)
{
	ISO_DIRECTORY* start = FindDirectory(path, 1);
	ISO_DIRECTORY* iso_dir = start;

	char* buffer;
	device->Read(iso_dir->locationLBA_LSB * ISO_SECTOR_SIZE, buffer, iso_dir->filesize_LSB);
	start = (ISO_DIRECTORY*)buffer;
	iso_dir = start;

	if (!iso_dir)
		return 0;

	while (iso_dir->length)
	{
		if (iso_dir->flags & FILE_FLAG_DIRECTORY)
		{
			dir_count++;
		}
		else
		{
			file_count++;
		}

		iso_dir = (ISO_DIRECTORY*)((int)iso_dir + iso_dir->length);
		if ((int)iso_dir - (int)start > ISO_SECTOR_SIZE - sizeof(ISO_DIRECTORY))
		{
			iso_dir = (ISO_DIRECTORY*)((int)start + ISO_SECTOR_SIZE);
			start = iso_dir;
		}
	}

	return 1;
}

bool ISO_FS::List(char* path, FILE_INFO*& files, DIRECTORY_INFO*& dirs, int& file_count, int& dir_count)
{
	if (!Count(path, 0, file_count, dir_count))
		return 0;

	ISO_DIRECTORY* start = FindDirectory(path, 1);
	ISO_DIRECTORY* iso_dir = start;

	char* buffer;
	device->Read(iso_dir->locationLBA_LSB * ISO_SECTOR_SIZE, buffer, iso_dir->filesize_LSB);
	start = (ISO_DIRECTORY*)buffer;
	iso_dir = start;

	if (!iso_dir)
		return 0;

	if (file_count + dir_count == 0)
		return 1;

	files = new FILE_INFO[file_count];
	dirs = new DIRECTORY_INFO[dir_count];

	int f = 0;
	int d = 0;

	while (iso_dir->length)
	{
		char name[32];
		GetName(iso_dir, name);

		if (iso_dir->flags & FILE_FLAG_DIRECTORY)
		{
			ParseDirectory(iso_dir, path, &dirs[d++]);
		}
		else
		{
			ParseFile(iso_dir, path, &files[f++]);
		}

		iso_dir = (ISO_DIRECTORY*)((int)iso_dir + iso_dir->length);
		if ((int)iso_dir - (int)start > ISO_SECTOR_SIZE - sizeof(ISO_DIRECTORY))
		{
			iso_dir = (ISO_DIRECTORY*)((int)start + ISO_SECTOR_SIZE);
			start = iso_dir;
		}
	}

	return 1;
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

bool ISO_FS::ParseFile(ISO_DIRECTORY* iso_dir, char* path, FILE_INFO* file)
{
	file->name = new char[iso_dir->idLength + 1];
	file->path = new char[strlen(path) + 1];

	GetName(iso_dir, file->name);
	strcpy(file->path, path);

	file->size = iso_dir->filesize_LSB;
	file->location = iso_dir->locationLBA_LSB * ISO_SECTOR_SIZE;
	file->attributes = iso_dir->attrib;

	return 1;
}

bool ISO_FS::ParseDirectory(ISO_DIRECTORY* iso_dir, char* path, DIRECTORY_INFO* dir)
{
	dir->name = new char[iso_dir->idLength + 1];
	dir->path = new char[strlen(path) + 1];

	GetName(iso_dir, dir->name);
	strcpy(dir->path, path);

	dir->attributes = iso_dir->attrib;

	return 1;
}
