#include "iso.h"
#include "string.h"
#include "ata.h"
#include "debug.h"

ISO_PRIMARYDESC* ISO_FS::desc;
ISO_DIRECTORY* ISO_FS::root;

STATUS ISO_FS::Init()
{
	desc = new ISO_PRIMARYDESC;
	char** buf = (char**)&desc;
	ATA::Read(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, 0x10, *buf, 0x1000);
	
	ISO_TABLEENTRY* table = (ISO_TABLEENTRY*)desc->rootDirectoryEntry;
	
	buf = (char**)&root;
	ATA::Read(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, table->locationLBA, *buf, sizeof(ISO_DIRECTORY));
	return STATUS_SUCCESS;
}

ISO_DIRECTORY* ISO_FS::FindFile(char* filename)
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

			if (strcmp(name, filename) == 0)
				return dir;
		}

		dir = (ISO_DIRECTORY*)((char*)dir + dir->length);
	}

	return 0;
}

STATUS ISO_FS::ReadFile(ISO_DIRECTORY* file, char*& buffer)
{
	ATA::Read(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, file->locationLBA_LSB, buffer, file->filesize_LSB);
	return STATUS_SUCCESS;
}

STATUS ISO_FS::ReadFile(char* name, char*& buffer)
{
	ISO_DIRECTORY* file = FindFile(name);

	if (!file)
		return STATUS_FAILED;

	Debug::Print("%s %i\n", name, file->filesize_LSB);
	ATA::Read(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, file->locationLBA_LSB, buffer, file->filesize_LSB);
	return STATUS_SUCCESS;
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