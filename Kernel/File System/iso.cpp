#include "iso.h"
#include "string.h"
#include "disk.h"
#include "debug.h"

ISO_PRIMARYDESC* desc;
ISO_DIRECTORY* root;

STATUS ISO_FS::Init()
{
	char* buf;
	if (!Disk::Read(0x10, buf, 0x1000))
		return STATUS_FAILED;

	desc = (ISO_PRIMARYDESC*)buf;

	ISO_TABLE_ENTRY* table = (ISO_TABLE_ENTRY*)desc->rootDirectoryEntry;

	if (!Disk::Read(table->locationLBA, buf, ISO_SECTOR_SIZE * 2))
		return STATUS_FAILED;

	root = (ISO_DIRECTORY*)buf;

	/*int length = *(int*)desc->pathTableSize;
	ATA::Read(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, desc->locationLPathTable, buf, length);
	Debug::Dump(buf, length, 1);
	ISO_TABLE_ENTRY* entry = (ISO_TABLE_ENTRY*)buf;


	while (entry->length)
	{
		char name[256];
		memset(name, 0, 256);
		memcpy(name, &entry->name, entry->length);

		Debug::Print("%i %s\n", entry->length, name);

		if (entry->length % 2)
			entry->length++;

		entry = (ISO_TABLE_ENTRY*)((char*)entry + entry->length - 3 + sizeof(ISO_TABLE_ENTRY) - 1);
	}

	Debug::Print("END");

	_asm cli
	_asm hlt*/

}

ISO_DIRECTORY* ISO_FS::FindFile(char* filename)
{
	ISO_DIRECTORY* start = (ISO_DIRECTORY*)root;
	ISO_DIRECTORY* dir = (ISO_DIRECTORY*)start;

	while (dir->length)
	{
		char name[256];
		memset(name, 0, 256);
		memcpy(name, &dir->identifier, dir->idLength);

		int length = strlen(name);
		if (length > 1)
		{
			//Debug::Print("N: %s\n", name);

			if (name[length - 3] == ';')
				name[length - 3] = 0;

			if (strcmp(name, filename) == 0)
				return dir;
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

uint32 ISO_FS::ReadFile(ISO_DIRECTORY* file, char*& buffer)
{
	if (!Disk::Read(file->locationLBA_LSB, buffer, file->filesize_LSB))
		return 0;

	return file->filesize_LSB;
}

uint32 ISO_FS::ReadFile(char* name, char*& buffer)
{
	ISO_DIRECTORY* file = FindFile(name);

	if (!file)
		return STATUS_FAILED;

	Debug::Print("%s %i\n", name, file->filesize_LSB);
	if (!Disk::Read(file->locationLBA_LSB, buffer, file->filesize_LSB))
		return 0;

	return file->filesize_LSB;
}

int ISO_FS::ListFiles(char* path)
{
	ISO_DIRECTORY* dir = (ISO_DIRECTORY*)root;

	while (dir->length != 0)
	{
		char name[256];
		memset(name, 0, 256);
		memcpy(name, &dir->identifier, dir->idLength);

		int length = strlen(name);
		if (length > 1)
		{
			if (name[length - 3] == ';')
				name[length - 3] = 0;

			Debug::Print("%s\n", name);
		}

		dir = (ISO_DIRECTORY*)((uint8*)dir + dir->length);
	}

	return 0;
}