#pragma once
#include "stdio.h"
#include "path.h"
#include "list.h"

#define INODE_PERM_OX (1 << 0)
#define INODE_PERM_OW (1 << 1)
#define INODE_PERM_OR (1 << 2)
#define INODE_PERM_GX (1 << 3)
#define INODE_PERM_GW (1 << 4)
#define INODE_PERM_GR (1 << 5)
#define INODE_PERM_UX (1 << 6)
#define INODE_PERM_UW (1 << 7)
#define INODE_PERM_UR (1 << 8)
#define INODE_PERM_STICKY (1 << 9)
#define INODE_PERM_SGID (1 << 10)
#define INODE_PERM_SUID (1 << 11)

#define INODE_TYPE_FIFO 0x1000
#define INODE_TYPE_CHAR 0x2000
#define INODE_TYPE_DIRECTORY 0x4000
#define INODE_TYPE_BLOCK 0x6000
#define INODE_TYPE_REGULAR 0x8000
#define INODE_TYPE_SYMLINK 0xA000
#define INODE_TYPE_SOCKET 0xC000

class Driver;
class FileSystem;

enum FILE_TYPE
{
    FILE_TYPE_REGULAR,
    FILE_TYPE_DIRECTORY,
    FILE_TYPE_BLOCK,
    FILE_TYPE_CHAR,
    FILE_TYPE_PIPE
};

struct INODE
{
    uint32 ino = 0;
    uint16 type = 0;
    size_t size = 0;
};

struct DENTRY
{
    Driver *owner = 0;
    INODE *inode = 0;
    uint16 type = 0;
    char *name = 0;
    DENTRY *parent = 0;
    List<DENTRY *> children;
};

struct FILE
{
    fpos_t pos = 0;
    DENTRY *dentry = 0;
    uint16 flags = 0;

    FILE() {}

    FILE(DENTRY *dentry)
    {
        this->dentry = dentry;
    }
};
