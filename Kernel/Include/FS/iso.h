#pragma once
#include <FS/filesystem.h>
#include <time.h>
#include <types.h>

#define ISO_SECTOR_SIZE         2048
#define ISO_FLAG_HIDDEN         (1 << 0)
#define ISO_FLAG_DIRECTORY      (1 << 1)
#define ISO_FLAG_ASSOSCIATED    (1 << 2)
#define ISO_FLAG_EXTENDED       (1 << 3)
#define ISO_FLAG_EXTENDED_OWNER (1 << 4)
#define ISO_FLAG_NOT_FINAL      (1 << 7)

#define ISO_NM_CONTINUE 0
#define ISO_NM_CURRENT  1
#define ISO_NM_PARENT   2

#define ISO_TF_CREATION   (1 << 0)
#define ISO_TF_MODIFY     (1 << 1)
#define ISO_TF_ACCESS     (1 << 2)
#define ISO_TF_ATTRIBUTES (1 << 3)
#define ISO_TF_BACKUP     (1 << 4)
#define ISO_TF_EXPIRATION (1 << 5)
#define ISO_TF_EFFECTIVE  (1 << 6)
#define ISO_TF_LONGFORM   (1 << 7)

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
    uint32 location;
    uint16 parentDir;
    char name;
} __attribute__((packed));

struct ISO_DIRECTORY
{
    uint8 length;
    uint8 attrib;
    uint32 location_LSB;
    uint32 location_MSB;
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

struct ISO_RR_DATA
{
    const char *name = 0;
    uint8 name_len = 0;
    mode_t mode = 0;
    nlink_t links = 0;
    uid_t uid = 0;
    gid_t gid = 0;
    ino_t ino = 0;
    time_t atime = 0;
    time_t ctime = 0;
    time_t mtime = 0;
};

class IsoFS : public FileSystem
{
  private:
    BlockDriver *driver;
    ISO_PRIMARY_DESC *desc;
    ISO_DIRECTORY *root;

  public:
    IsoFS() { name = "iso"; }

    int Mount(BlockDriver *driver);
    int GetChildren(DENTRY *parent, const char *find_name);
    int Read(FILE *file, void *buf, size_t size);

  private:
    void ReadRockRidge(ISO_DIRECTORY *dir, ISO_RR_DATA *rr);
    void GetName(ISO_DIRECTORY *dir, const ISO_RR_DATA *rr, char *buf);
    int GetMode(ISO_DIRECTORY *dir);
    time_t GetTime(ISO_DIRECTORY *dir);
    INODE *GetInode(ISO_DIRECTORY *dir, const ISO_RR_DATA *rr, DENTRY *dentry);
    int ReadBlock(int block, void *buf, size_t size);
};