#include "devfs.h"
#include "vfs.h"
#include "string.h"
#include "Drivers/driver.h"

int DevFS::Mount(BlockDriver *driver, DENTRY *root_dentry)
{
    root_dentry->inode = VFS::AllocInode(root_dentry);
    return 0;
}

int DevFS::Open(FILE *file)
{
    Driver *driver = GetDriver(file);

    if (!driver)
        return -1;

    return driver->Open(file);
}

int DevFS::Close(FILE *file)
{
    Driver *driver = GetDriver(file);

    if (!driver)
        return -1;

    return driver->Close(file);
}

int DevFS::Read(FILE *file, void *buf, size_t size)
{
    Driver *driver = GetDriver(file);

    if (!driver)
        return -1;

    return driver->Read(file, buf, size);
}

int DevFS::Write(FILE *file, const void *buf, size_t size)
{
    Driver *driver = GetDriver(file);

    if (!driver)
        return -1;

    return driver->Write(file, buf, size);
}

int DevFS::Seek(FILE *file, long offset, int origin)
{
    Driver *driver = GetDriver(file);

    if (!driver)
        return -1;

    return driver->Seek(file, offset, origin);
}

int DevFS::GetChildren(DENTRY *parent, const char *find_name)
{
    Driver *driver = DriverManager::GetDriver();

    while (driver->next)
    {
        DRIVER_TYPE driver_type = driver->type;

        if (driver_type == DRIVER_TYPE_BLOCK || driver_type == DRIVER_TYPE_CHAR)
        {
            if (!find_name || strcmp(driver->name, find_name) == 0)
            {
                DENTRY *dentry = VFS::AllocDentry(parent, driver->name);

                if (!dentry->inode)
                {
                    int type = driver_type == DRIVER_TYPE_BLOCK ? INODE_TYPE_BLOCK : INODE_TYPE_CHAR;
                    dentry->type = type;
                    dentry->inode = VFS::AllocInode(dentry);
                    dentry->inode->ino = driver->id;
                    dentry->inode->type = type;
                    VFS::AddDentry(parent, dentry);

                    if (find_name)
                        break;
                }
            }
        }

        driver = driver->next;
    }

    return 0;
}

Driver *DevFS::GetDriver(FILE *file)
{
    int ino = file->dentry->inode->ino;
    return DriverManager::GetDriver(ino);
}
