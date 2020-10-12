#include <FS/ext2.h>
#include <debug.h>
#include <string.h>

int Ext2FS::Mount(BlockDriver *driver)
{
    this->driver = driver;
    superblock = new EXT2_SUPERBLOCK;

    driver->Read(EXT_SUPERBLOCK_OFFSET, superblock, sizeof(EXT2_SUPERBLOCK));

    if (superblock->signature != EXT2_SIGNATURE) {
        kprintf("Invalid ext2 signature %p\n", superblock->signature);
        return -1;
    }

    if (superblock->version_major < 1) {
        superblock->first_free = 11;
        superblock->inode_size = 128;
    }

    block_size = 1024 << superblock->log2_block;
    block_groups = DIV_CEIL(superblock->blocks, superblock->group_blocks);
    int block_groups2 = DIV_CEIL(superblock->inodes, superblock->group_inodes);

    if (superblock->state != EXT_STATE_CLEAN) {
        kprintf("Ext2 state error %d\n", superblock->state);
        return -1;
    }

    group_table = new EXT_BLOCK_GROUP[block_groups];
    driver->Read(EXT_GROUP_DESCRIPTOR_TABLE_OFFSET, group_table,
                 block_groups * sizeof(EXT_BLOCK_GROUP));

    root_dentry->inode = ReadInode(EXT_ROOT_INODE, root_dentry);
    return 0;
}

int Ext2FS::GetChildren(DENTRY *parent, const char *find_name)
{
    EXT_INODE *raw_inode = ReadRawInode(parent->inode->ino);

    if (!raw_inode)
        return -1;

    char *buf = new char[raw_inode->size];
    ReadBlock(raw_inode->ptr_direct0, buf, raw_inode->size);
    char *ptr = buf;

    char name_buf[256];

    while (true) {
        EXT_DIRENT *dirent = (EXT_DIRENT *)ptr;

        if (dirent->ino == 0)
            break;

        ptr += dirent->size;

        if (!find_name || memcmp(find_name, dirent->name, dirent->name_len) == 0) {
            memcpy(name_buf, dirent->name, dirent->name_len);
            name_buf[dirent->name_len] = 0;

            DENTRY *dentry = VFS::AllocDentry(parent, name_buf);

            if (!dentry->inode) {
                ReadInode(dirent->ino, dentry);
                VFS::AddDentry(parent, dentry);
            }

            if (find_name)
                break;
        }
    }

    delete[] buf;
    return 0;
}

int Ext2FS::Read(FILE *file, void *buf, size_t size)
{
    DENTRY *dentry = file->dentry;
    uint32 ino = dentry->inode->ino;
    EXT_INODE *raw_inode = ReadRawInode(ino);
    int ret = ReadBlock(raw_inode->ptr_direct0, buf, size);
    delete[] raw_inode;
    return ret;
}

INODE *Ext2FS::ReadInode(int ino, DENTRY *dentry)
{
    EXT_INODE *raw_inode = ReadRawInode(ino);

    if (!raw_inode)
        return 0;

    INODE *inode = VFS::AllocInode(dentry);
    inode->ino = ino;
    inode->mode = raw_inode->type;
    inode->size = raw_inode->size;
    inode->uid = raw_inode->uid;
    inode->gid = raw_inode->gid;
    inode->atime = raw_inode->atime;
    inode->ctime = raw_inode->ctime;
    inode->mtime = raw_inode->mtime;
    inode->dtime = raw_inode->dtime;

    delete[] raw_inode;
    return inode;
}

EXT_INODE *Ext2FS::ReadRawInode(int ino)
{
    EXT_INODE *raw_inode = (EXT_INODE *)(new char[superblock->inode_size]);

    int block_group = (ino - 1) / superblock->group_inodes;
    int group_index = (ino - 1) % superblock->group_inodes;
    int offset = group_index * superblock->inode_size;

    EXT_BLOCK_GROUP *group = &group_table[block_group];
    driver->Read(group->inode_table * this->block_size + offset, raw_inode, superblock->inode_size);
    return raw_inode;
}

int Ext2FS::ReadBlock(int block, void *buf, size_t size)
{
    return driver->Read(block * this->block_size, buf, size);
}
