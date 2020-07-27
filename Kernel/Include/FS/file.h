#pragma once
#include <stdio.h>
#include <FS/path.h>
#include <list.h>

class Driver;
class FileSystem;

struct INODE
{
    ino_t ino = 0;
    mode_t mode = 0;
    off_t size = 0;
    uid_t uid = 0;
    gid_t gid = 0;
    time_t atime = 0;
    time_t ctime = 0;
    time_t mtime = 0;
    time_t dtime = 0;
};

struct DENTRY
{
    Driver *owner = 0;
    INODE *inode = 0;
    char *name = 0;
    DENTRY *parent = 0;
    List<DENTRY *> children;
    int refs = 0;
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
