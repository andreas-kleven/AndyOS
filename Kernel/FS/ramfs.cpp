#include <FS/ramfs.h>
#include <FS/vfs.h>
#include <fcntl.h>
#include <math.h>
#include <stat.h>
#include <string.h>

#define RAMFS_ENT(dentry) ((RAMFS_ENTRY *)(dentry)->inode->custom_data);

int RamFS::Mount(BlockDriver *driver)
{
    VFS::AllocInode(root_dentry, 1, S_IFDIR);
    RAMFS_ENTRY *entry = new RAMFS_ENTRY();
    entry->name = strdup(root_dentry->name);
    entry->inode = *root_dentry->inode;
    root_dentry->inode->custom_data = entry;
    next_inode = 2;
    return 0;
}

int RamFS::Open(FILE *file)
{
    if (file->flags & O_TRUNC) {
        RAMFS_ENTRY *entry = RAMFS_ENT(file->dentry);

        if (!entry)
            return -1;

        RAMFS_CHUNK *chunk = entry->first_chunk;

        while (chunk) {
            RAMFS_CHUNK *next = chunk->next;
            delete chunk->data;
            delete chunk;
            chunk = next;
        }

        entry->first_chunk = 0;
        entry->last_chunk = 0;
    }

    return 0;
}

int RamFS::Close(FILE *file)
{
    return 0;
}

int RamFS::Read(FILE *file, void *buf, size_t size)
{
    RAMFS_ENTRY *entry = RAMFS_ENT(file->dentry);

    if (!entry)
        return -1;

    if (!entry->first_chunk)
        return 0;

    char *ptr = (char *)buf;
    size_t read = 0;
    size_t offset = 0;
    RAMFS_CHUNK *chunk = SeekChunk(entry, file, &offset);

    while (size && chunk) {
        size_t length = min(size, chunk->length - offset);

        if (length == 0)
            break;

        memcpy(ptr, (char *)chunk->data + offset, length);

        ptr += length;
        size -= length;
        read += length;

        if (length < chunk->length)
            offset = chunk->length - length;
        else
            chunk = chunk->next;
    }

    return read;
}

int RamFS::Write(FILE *file, const void *buf, size_t size)
{
    RAMFS_ENTRY *entry = RAMFS_ENT(file->dentry);

    if (!entry)
        return -1;

    char *ptr = (char *)buf;
    size_t written = 0;
    size_t offset = 0;
    RAMFS_CHUNK *chunk = SeekChunk(entry, file, &offset);

    if (chunk) {
        size_t available = chunk->capacity - offset;

        if (available > 0) {
            size_t length = min(available, size);
            memcpy((char *)chunk->data + offset, ptr, length);
            ptr += length;
            size -= length;
            written += length;
            chunk->length += length;
            chunk = chunk->next;
        }
    }

    while (size) {
        if (!chunk) {
            chunk = new RAMFS_CHUNK();
            chunk->data = new char[RAMFS_CHUNK_SIZE];
            chunk->capacity = RAMFS_CHUNK_SIZE;
            chunk->length = 0;
        }

        size_t length = min(size, chunk->capacity);
        chunk->length = max(length, chunk->length);

        memcpy(chunk->data, ptr, length);
        ptr += length;
        size -= length;
        written += length;

        if (entry->last_chunk) {
            entry->last_chunk->next = chunk;
            entry->last_chunk = chunk;
        } else {
            entry->first_chunk = chunk;
            entry->last_chunk = chunk;
        }
    }

    // TODO
    entry->inode.size += written;
    file->dentry->inode->size += written;
    return written;
}

int RamFS::Seek(FILE *file, long offset, int origin)
{
    return 0;
}

int RamFS::Create(DENTRY *parent, const char *name, mode_t mode)
{
    RAMFS_ENTRY *parent_entry = RAMFS_ENT(parent);

    if (!parent_entry)
        return -1;

    RAMFS_ENTRY *entry = new RAMFS_ENTRY();
    entry->name = strdup(name);
    entry->inode.mode = mode;
    entry->inode.custom_data = entry;
    entry->inode.ino = next_inode++;
    parent_entry->children.Add(entry);
    return 0;
}

int RamFS::GetChildren(DENTRY *parent, const char *find_name)
{
    RAMFS_ENTRY *parent_entry = RAMFS_ENT(parent);

    if (!parent_entry)
        return -1;

    for (int i = 0; i < parent_entry->children.Count(); i++) {
        RAMFS_ENTRY *entry = parent_entry->children[i];

        if (!find_name || strcmp(entry->name, find_name) == 0) {
            DENTRY *dentry = VFS::AllocDentry(parent, entry->name);
            INODE *inode = VFS::AllocInode(dentry, entry->inode.ino, entry->inode.mode);
            *inode = entry->inode;
            VFS::AddDentry(parent, dentry);
        }
    }

    return 0;
}

RAMFS_CHUNK *RamFS::SeekChunk(RAMFS_ENTRY *entry, FILE *file, size_t *offset)
{
    fpos_t skipped = 0;
    RAMFS_CHUNK *chunk = (RAMFS_CHUNK *)entry->first_chunk;

    while (chunk && skipped < file->pos) {
        if (skipped + chunk->length > (size_t)file->pos) {
            *offset = file->pos - skipped;
            skipped = file->pos;
            break;
        }

        skipped += chunk->length;
        chunk = chunk->next;
    }

    return chunk;
}
