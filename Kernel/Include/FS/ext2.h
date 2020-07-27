#pragma once
#include <FS/vfs.h>
#include <FS/filesystem.h>
#include <types.h>
#include <list.h>

#define EXT_SUPERBLOCK_OFFSET 1024
#define EXT_GROUP_DESCRIPTOR_TABLE_OFFSET 2048
#define EXT_STATE_CLEAN 1
#define EXT_STATE_ERRORS 2
#define EXT2_SIGNATURE 0xEF53
#define EXT_ROOT_INODE 2
#define EXT_INODE_DIRECT_COUNT 12

#define EXT_DIRENT_UNKNOWN 0
#define EXT_DIRENT_REGULAR 1
#define EXT_DIRENT_DIRECTORY 2
#define EXT_DIRENT_CHAR 3
#define EXT_DIRENT_BLOCK 4
#define EXT_DIRENT_FIFO 5
#define EXT_DIRENT_SOCKET 6
#define EXT_DIRENT_SYMLINK 7

#define EXT_PERM_OX (1 << 0)
#define EXT_PERM_OW (1 << 1)
#define EXT_PERM_OR (1 << 2)
#define EXT_PERM_GX (1 << 3)
#define EXT_PERM_GW (1 << 4)
#define EXT_PERM_GR (1 << 5)
#define EXT_PERM_UX (1 << 6)
#define EXT_PERM_UW (1 << 7)
#define EXT_PERM_UR (1 << 8)
#define EXT_PERM_STICKY (1 << 9)
#define EXT_PERM_SGID (1 << 10)
#define EXT_PERM_SUID (1 << 11)

#define EXT_INODE_FIFO 0x1000
#define EXT_INODE_CHAR 0x2000
#define EXT_INODE_DIRECTORY 0x4000
#define EXT_INODE_BLOCK 0x6000
#define EXT_INODE_REGULAR 0x8000
#define EXT_INODE_SYMLINK 0xA000
#define EXT_INODE_SOCKET 0xC000

struct EXT2_SUPERBLOCK
{
    uint32 inodes;
    uint32 blocks;
    uint32 reserved_blocks;
    uint32 free_blocks;
    uint32 free_inodes;
    uint32 superblocks;
    uint32 log2_block;
    uint32 log2_fragment;
    uint32 group_blocks;
    uint32 group_fragments;
    uint32 group_inodes;
    uint32 last_mount;
    uint32 last_write;
    uint16 check_mount;
    uint16 check_allowed;
    uint16 signature;
    uint16 state;
    uint16 error;
    uint16 version_minor;
    uint32 last_check;
    uint32 check_interval;
    uint32 os;
    uint32 version_major;
    uint16 uid;
    uint16 gid;

    // Extended
    uint32 first_free;
    uint16 inode_size;
} __attribute__((packed));

struct EXT_BLOCK_GROUP
{
    uint32 block_bitmap;
    uint32 inode_bitmap;
    uint32 inode_table;
    uint16 free_blocks;
    uint16 free_inodes;
    uint16 directories;
    uint8 unused[14];
} __attribute__((packed));

struct EXT_INODE
{
    uint16 type;
    uint16 uid;
    uint32 size;
    uint32 atime;
    uint32 ctime;
    uint32 mtime;
    uint32 dtime;
    uint16 gid;
    uint16 hard_links;
    uint32 sectors;
    uint32 flags;
    uint32 os1;
    uint32 ptr_direct0;
    uint32 ptr_direct1;
    uint32 ptr_direct2;
    uint32 ptr_direct3;
    uint32 ptr_direct4;
    uint32 ptr_direct5;
    uint32 ptr_direct6;
    uint32 ptr_direct7;
    uint32 ptr_direct8;
    uint32 ptr_direct9;
    uint32 ptr_direct10;
    uint32 ptr_direct11;
    uint32 ptr_single;
    uint32 ptr_double;
    uint32 ptr_triple;
    uint32 generation;
    uint32 attribute_block;
    uint32 size_hi;
    uint32 fragment_block;
    uint32 os2;
} __attribute__((packed));

struct EXT_DIRENT
{
    uint32 ino;
    uint16 size;
    uint8 name_len;
    uint8 type;
    char name[256];
} __attribute__((packed));

class Ext2FS : public FileSystem
{
private:
    BlockDriver *driver;
    EXT2_SUPERBLOCK *superblock;
    EXT_BLOCK_GROUP *group_table;
    int block_size;
    int block_groups;

public:
    Ext2FS()
    {
        name = "ext2";
    }

    int Mount(BlockDriver *driver);
    int GetChildren(DENTRY *parent, const char *find_name);
    int Read(FILE *file, void *buf, size_t size);

private:
    INODE *ReadInode(int ino, DENTRY *dentry);
    EXT_INODE *ReadRawInode(int ino);
    int ReadBlock(int block, void *buf, size_t size);
};
