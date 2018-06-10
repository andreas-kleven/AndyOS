#include "iso.h"
#include "string.h"
#include "disk.h"
#include "debug.h"

#define FILE_FLAG_HIDDEN			(1 << 0)
#define FILE_FLAG_DIRECTORY			(1 << 1)
#define FILE_FLAG_ASSOSCIATED		(1 << 2)
#define FILE_FLAG_EXTENDED			(1 << 3)
#define FILE_FLAG_EXTENDED_OWNER	(1 << 4)
#define FILE_FLAG_NOT_FINAL			(1 << 7)

ISO_FS* ISO_FS::instance;

STATUS ISO_FS::Init()
{
	instance = new ISO_FS();
	return STATUS_SUCCESS;
}

ISO_FS::ISO_FS()
{
	char* buf;
	if (!Disk::Read(0x10, buf, 0x1000))
		return;

	desc = (ISO_PRIMARYDESC*)buf;

	ISO_TABLE_ENTRY* table = (ISO_TABLE_ENTRY*)desc->rootDirectoryEntry;

	if (!Disk::Read(table->locationLBA, buf, ISO_SECTOR_SIZE * 2))
		return;

	root = (ISO_DIRECTORY*)buf;
}

ISO_DIRECTORY* ISO_FS::FindDirectory(char* path, bool isDir)
{
	ISO_DIRECTORY* start = (ISO_DIRECTORY*)root;
	ISO_DIRECTORY* dir = start;

	char* saveptr;
	char* part = strtok_r(path, "/", &saveptr);
	char* next = strtok_r(0, "/", &saveptr);

	char* buffer;

	while (part && dir->length)
	{
		char name[256];
		memset(name, 0, 256);
		memcpy(name, &dir->identifier, dir->idLength);

		int length = strlen(name);
		if (length > 1)
		{
			//Debug::Print("N: %s\t%s\t%ux\n", name, part, dir->flags);

			if (name[length - 3] == ';')
				name[length - 3] = 0;

			if (strcmp(name, part) == 0)
			{
				if (dir->flags & FILE_FLAG_DIRECTORY)
				{
					if (next)
					{
						Disk::Read(dir->locationLBA_LSB, buffer, dir->filesize_LSB);
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

	dir->name = new char[iso_dir->idLength];
	memcpy(dir->name, &iso_dir->identifier, iso_dir->idLength);
	strcpy(dir->path, path);

	dir->attributes = iso_dir->attrib;

	return 1;
}

bool ISO_FS::GetFile(DIRECTORY_INFO* dir, char* path, FILE_INFO* file)
{
	ISO_DIRECTORY* iso_dir = FindDirectory(path, false);

	if (!iso_dir)
		return 0;

	file->name = new char[iso_dir->idLength];
	memcpy(file->name, &iso_dir->identifier, iso_dir->idLength);
	strcpy(file->path, path);

	file->size = iso_dir->filesize_LSB;
	file->location = iso_dir->locationLBA_LSB;
	file->attributes = iso_dir->attrib;

	return 1;
}

bool ISO_FS::ReadFile(FILE_INFO* file, char*& buffer)
{
	if (!Disk::Read(file->location, buffer, file->size))
		return 0;

	return 1;
}

bool ISO_FS::WriteFile(FILE_INFO* file, void* data, uint32 length)
{
	return 0;
}