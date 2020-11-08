#include <FS/iso.h>
#include <FS/vfs.h>
#include <debug.h>
#include <driver.h>
#include <string.h>

int IsoFS::Mount(BlockDriver *driver)
{
    this->driver = driver;

    desc = (ISO_PRIMARY_DESC *)(new char[0x1000]);
    if (!ReadBlock(0x10, desc, 0x1000))
        return -1;

    ISO_DIRECTORY *table = &desc->rootDirectory;

    root = (ISO_DIRECTORY *)(new char[table->filesize_LSB]);
    if (!ReadBlock(table->location_LSB, root, table->filesize_LSB))
        return -1;

    ISO_RR_DATA rr_data;
    ReadRockRidge(root, &rr_data);
    root_dentry->inode = GetInode(root, &rr_data, root_dentry);
    return 0;
}

int IsoFS::GetChildren(DENTRY *parent, const char *find_name)
{
    INODE *inode = parent->inode;
    ISO_DIRECTORY *dir = root;
    ISO_DIRECTORY *end = (ISO_DIRECTORY *)((uint8 *)root + desc->rootDirectory.filesize_LSB);

    if (inode->ino > 1) {
        dir = (ISO_DIRECTORY *)(new char[inode->size]);
        end = (ISO_DIRECTORY *)((uint8 *)dir + inode->size);

        if (!ReadBlock(inode->ino, dir, inode->size))
            return -1;
    }

    char name[256];

    while (dir < end) {
        ISO_RR_DATA rr_data;
        ReadRockRidge(dir, &rr_data);
        GetName(dir, &rr_data, name);

        if (!find_name || strcmp(name, find_name) == 0) {
            DENTRY *dentry = VFS::AllocDentry(parent, name);

            if (!dentry->inode) {
                dentry->inode = GetInode(dir, &rr_data, dentry);
                VFS::AddDentry(parent, dentry);
            }

            if (find_name)
                break;
        }

        dir = (ISO_DIRECTORY *)((size_t)dir + dir->length);
        size_t space_left = ISO_SECTOR_SIZE - ((size_t)dir % ISO_SECTOR_SIZE);

        if (!dir->length)
            dir = (ISO_DIRECTORY *)((size_t)dir + space_left);
    }

    return 0;
}

int IsoFS::Read(FILE *file, void *buf, size_t size)
{
    return driver->Read(file->pos + file->dentry->inode->ino * ISO_SECTOR_SIZE, buf, size);
}

void IsoFS::ReadRockRidge(ISO_DIRECTORY *dir, ISO_RR_DATA *rr)
{
    char *end = (char *)dir + dir->length;
    char *ptr = &dir->identifier + dir->idLength;

    if ((size_t)ptr % 2 == 1)
        ptr += 1;

    while (end - ptr > 3) {
        uint8 length = ptr[2];

        if (!length)
            return;

        if (ptr[0] == 'P' && ptr[1] == 'X') {
            rr->mode = *(mode_t *)&ptr[4];
            rr->links = *(nlink_t *)&ptr[12];
            rr->uid = *(uid_t *)&ptr[20];
            rr->gid = *(gid_t *)&ptr[28];
            rr->ino = *(ino_t *)&ptr[36];
        } else if (ptr[0] == 'N' && ptr[1] == 'M') {
            int flags = ptr[4];

            if (flags & ISO_NM_CURRENT) {
                rr->name = ".";
                rr->name_len = 1;
            } else if (flags & ISO_NM_PARENT) {
                rr->name = "..";
                rr->name_len = 2;
            } else {
                rr->name = &ptr[5];
                rr->name_len = length - 5;
            }
        } else if (ptr[0] == 'T' && ptr[1] == 'F') {
            // TODO
            /*int flags = ptr[4];
            uint64 *timestamps = (uint64 *)&ptr[5];
            int idx = 0;

            bool long_format = flags & ISO_TF_LONGFORM;

            if (flags & ISO_TF_CREATION)
                    idx++;

            if (flags & ISO_TF_MODIFY)
                    rr->mtime = (time_t)timestamps[idx++];

            if (flags & ISO_TF_ACCESS)
                    rr->atime = (time_t)timestamps[idx++];

            if (flags & ISO_TF_ATTRIBUTES)
                    rr->ctime = (time_t)timestamps[idx++];*/
        }

        ptr += length;
    }
}

void IsoFS::GetName(ISO_DIRECTORY *dir, const ISO_RR_DATA *rr, char *buf)
{
    if (rr->name && rr->name_len < 256) {
        memcpy(buf, rr->name, rr->name_len);
        buf[rr->name_len] = 0;
    } else {
        memcpy(buf, &dir->identifier, dir->idLength);
        buf[dir->idLength] = 0;
        stolower(buf);

        int length = strlen(buf);

        if (length <= 2) {
            if (buf[0] == 0) {
                strcpy(buf, ".");
            } else if (buf[0] == 1) {
                strcpy(buf, "..");
            }
        }

        if (length >= 2) {
            if (buf[length - 2] == ';')
                buf[length - 2] = 0;
        }

        if (length >= 3) {
            if (buf[length - 3] == '.')
                buf[length - 3] = 0;
        }
    }
}

int IsoFS::GetMode(ISO_DIRECTORY *dir)
{
    mode_t mode = 0;

    mode |= S_IRWXU | S_IRWXG | S_IRWXO;

    if (dir->flags & ISO_FLAG_DIRECTORY)
        mode |= S_IFDIR;
    else
        mode |= S_IFREG;

    return mode;
}

time_t IsoFS::GetTime(ISO_DIRECTORY *dir)
{
    ISO_DATE *iso = &dir->recording_date;
    struct tm t = mktime_tm(iso->second + 1, iso->minute + 1, iso->hour + 1, iso->day, iso->month,
                            iso->years + 1900);
    return mktime(&t);
}

INODE *IsoFS::GetInode(ISO_DIRECTORY *dir, const ISO_RR_DATA *rr, DENTRY *dentry)
{
    ino_t ino = dir->location_LSB;
    mode_t mode = rr->mode ? rr->mode : GetMode(dir);
    INODE *inode = VFS::AllocInode(dentry, ino, mode);

    inode->size = dir->filesize_LSB;
    inode->uid = rr->uid;
    inode->gid = rr->gid;

    time_t rec_time = GetTime(dir);
    inode->atime = rr->atime ? rr->atime : rec_time;
    inode->ctime = rr->ctime ? rr->ctime : rec_time;
    inode->mtime = rr->mtime ? rr->mtime : rec_time;

    if (dir == root)
        inode->ino = 1;

    return inode;
}

int IsoFS::ReadBlock(int block, void *buf, size_t size)
{
    return driver->Read(block * ISO_SECTOR_SIZE, buf, size);
}
