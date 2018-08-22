#pragma once
#include "types.h"
#include "filesystem.h"

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
} __attribute__((packed));

enum ISO_VOLUME_DESC_TYPE : uint8
{
	BOOT_RECORD = 0,
	PRIMARY = 1,
	SUPPLEMENTARY = 2,
	PARTITION = 2,
	TERMINATOR = 255
};

struct ISO_TABLE_ENTRY
{
	uint8 length;
	uint8 attrib;
	uint32 locationLBA;
	uint16 parentDir;
	char name;
} __attribute__((packed));

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
	char identifier;
} __attribute__((packed));

struct ISO_VOLUME_DESC
{
	ISO_VOLUME_DESC_TYPE type;
	char identifier[5];
	uint8 version;
};

struct ISO_PRIMARY_DESC
{
	ISO_VOLUME_DESC_TYPE type;
	char identifier[5];
	uint8 version;
	char unused1;
	char systemIdentifier[32];
	char volumeIdentifier[32];
	char unused2[8];
	uint32 volumeSpaceSize_LSB;
	uint32 volumeSpaceSize_MSB;
	char unused3[32];
	uint16 volumeSetSize_LSB;
	uint16 volumeSetSize_MSB;
	uint16 volumeSequenceNumber_LSB;
	uint16 volumeSequenceNumber_MSB;
	uint16 logicalBlockSize_LSB;
	uint16 logicalBlockSize_MSB;
	uint32 pathTableSize_LSB;
	uint32 pathTableSize_MSB;
	uint32 locationLPathTable;
	uint32 locationOptionalLPathTable;
	uint32 locationMPathTable;
	uint32 locationOptionalMPathTable;
	ISO_DIRECTORY rootDirectory;
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
	uint8 fileStructureVersion;
	char unused4;
	char applicationUsed[512];
	char reserved[653];
} __attribute__((packed));

class ISO_FS : public FileSystem
{
private:
	BlockDriver* driver;

public:
	ISO_PRIMARY_DESC* desc;
	ISO_DIRECTORY* root;

	ISO_FS(BlockDriver* driver);

    int Read(FILE* file, char* buf, size_t size);
	bool GetFile(const Path* path, FNODE* node);

private:
	ISO_DIRECTORY* FindDirectory(const Path* path);
	void GetName(ISO_DIRECTORY* dir, char* buf);
};