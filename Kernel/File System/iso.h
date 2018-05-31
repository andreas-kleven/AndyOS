#pragma once
#include "definitions.h"

#define ISO_SECTOR_SIZE 2048

struct ISO_DATE
{
	uint8 years;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 minute;
	uint8 second;
	uint8 offset;
};

struct ISO_PRIMARYDESC
{
	char type;
	char identifier[5];
	char version;
	char unused;
	char systemIdentifier[32];
	char volumeIdentifier[32];
	char unused2[8];
	char volumeSpaceSize[8];
	char unused3[32];
	int volumeSetSize;
	int volumeSequenceNumber;
	int logicalBlockSize;
	char pathTableSize[8];
	int locationLPathTable;
	int locationOptionalLPathTable;
	int locationMPathTable;
	int locationOptionalMPathTable;
	char rootDirectoryEntry[34];
	char volumeSetID[128];
	char publisherID[128];
	char dataPreparerID[128];
	char applicationID[128];
	char copyrightFileID[38];
	char abstractFileID[36];
	char BibliographicFileID[37];
	char volumeCreationDateTime[17];
	char volumeModificationDateTime[17];
	char volumeExpirationDateTime[17];
	char volumeEffectiveDateTime[17];
	char fileStructureVersion;
	char unused4;
	char applicationUsed[512];
	char reserved[653];
};

struct ISO_TABLE_ENTRY
{
	uint8 length;
	uint8 attrib;
	uint32 locationLBA;
	uint16 parentDir;
	char* name;
};

struct ISO_DIRECTORY
{
	uint8 length;
	uint8 attrib;
	uint32 locationLBA_LSB;
	uint32 locationLBA_MSB;
	uint32 filesize_LSB;
	uint32 filesize_MSB;
	ISO_DATE recording_date;
	uint8 flags;
	uint8 interleaveUnitSize;
	uint8 interleaveGapSize;
	uint16 volSeqNum_LSB;
	uint16 volSeqNum_MSB;
	uint8 idLength;
	char* identifier;
};

class ISO_FS
{
public:
	static STATUS Init();

	static ISO_DIRECTORY* FindFile(char* filename);
	static uint32 ReadFile(ISO_DIRECTORY* file, char*& buffer);
	static uint32 ReadFile(char* name, char*& buffer);
	static int ListFiles(char* path);
};