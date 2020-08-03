#pragma once
#include <stdio.h>
#include <list.h>
#include <sync.h>

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
    dev_t dev = 0;
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
    Mutex lock;
    fpos_t pos = 0;
    DENTRY *dentry = 0;
    int flags = 0;

    FILE() {}

    FILE(DENTRY *dentry)
    {
        this->dentry = dentry;
    }
};
