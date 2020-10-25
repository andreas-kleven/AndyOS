#include <FS/filesystem.h>
#include <FS/pipe.h>
#include <FS/pipefs.h>
#include <FS/sockfs.h>
#include <FS/vfs.h>
#include <Kernel/timer.h>
#include <Process/dispatcher.h>
#include <Process/process.h>
#include <Process/scheduler.h>
#include <driver.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <math.h>
#include <string.h>

namespace VFS {
DENTRY *root_dentry = 0;
PipeFS *pipefs = 0;
SockFS *sockfs = 0;

char *TraversePath(DENTRY *dentry, char *buf, size_t size)
{
    if (size <= 1)
        return 0;

    char *ptr = &buf[size];
    *--ptr = 0;

    while (dentry) {
        if (!dentry->name)
            return ptr;

        if (*ptr != '/') {
            if (ptr <= buf)
                return 0;

            *--ptr = '/';
        }

        if (dentry != root_dentry) {
            int len = strlen(dentry->name);

            if (!len)
                return ptr;

            ptr -= len;

            if (ptr < buf)
                return 0;

            memcpy(ptr, dentry->name, len);
        }

        dentry = dentry->parent;
    }

    return ptr;
}

inline bool IsValidInode(FILE *file)
{
    return file && file->dentry && file->dentry->inode;
}

DENTRY *GetExistingChild(DENTRY *parent, const char *name)
{
    for (int i = 0; i < parent->children.Count(); i++) {
        DENTRY *child = parent->children[i];

        if (strcmp(child->name, name) == 0)
            return child;
    }

    return 0;
}

INODE *AllocInode(DENTRY *dentry, ino_t ino, mode_t mode)
{
    if (!dentry->inode)
        dentry->inode = new INODE();

    dentry->inode->ino = ino;
    dentry->inode->mode = mode;
    return dentry->inode;
}

DENTRY *AllocDentry(DENTRY *parent, const char *name)
{
    if (!name)
        return 0;

    if (parent) {
        DENTRY *dentry = GetExistingChild(parent, name);

        if (dentry)
            return dentry;
    }

    DENTRY *dentry = new DENTRY();
    dentry->name = strdup(name);
    return dentry;
}

void AddDentry(DENTRY *parent, DENTRY *child)
{
    if (child->name) {
        if (GetExistingChild(parent, child->name)) {
            kprintf("dentry already added %s\n", child->name);
            return;
        }
    }

    if (!child->inode) {
        kprintf("Missing inode\n");
    }

    child->parent = parent;
    child->owner = parent->owner;
    parent->children.Add(child);
}

int GetChildren(DENTRY *parent)
{
    return parent->owner->GetChildren(parent, 0);
}

DENTRY *GetChild(DENTRY *parent, const char *name)
{
    DENTRY *dentry = GetExistingChild(parent, name);

    if (dentry)
        return dentry;

    parent->owner->GetChildren(parent, name);
    return GetExistingChild(parent, name);
}

DENTRY *GetRoot()
{
    return root_dentry;
}

DENTRY *GetDentry(const char *path)
{
    DENTRY *current = root_dentry;

    if (!root_dentry)
        return 0;

    if (!path || !strlen(path))
        return 0;

    if (strlen(path) >= PATH_MAX)
        return 0;

    char buf[PATH_MAX];
    strcpy(buf, path);

    char *saveptr;
    char *part = strtok_r(buf, "/", &saveptr);

    while (part) {
        current = GetChild(current, part);

        if (!current)
            return 0;

        part = strtok_r(0, "/", &saveptr);
    }

    return current;
}

DENTRY *GetDentry(PROCESS *process, const char *path)
{
    if (!path || strlen(path) == 0)
        return (DENTRY *)-ENOENT;

    char pathbuf[PATH_MAX];
    const char *pwd = "/";

    if (process)
        pwd = TraversePath(process->pwd, pathbuf, sizeof(pathbuf));

    if (!pwd)
        return (DENTRY *)-ENAMETOOLONG;

    char fullpath[strlen(pwd) + strlen(path) + 2];

    if (path[0] == '/')
        strcpy(fullpath, path);
    else
        sprintf(fullpath, "%s%s", pwd, path);

    if (pathcanon(fullpath, pathbuf, sizeof(pathbuf))) {
        return (DENTRY *)-ENAMETOOLONG;
    }

    DENTRY *dentry = GetDentry(pathbuf);

    if (!dentry)
        return (DENTRY *)-ENOENT;

    return dentry;
}

int Mount(BlockDriver *driver, FileSystem *fs, const char *mount_point)
{
    if (!fs)
        return -1;

    if (!mount_point || !strlen(mount_point))
        return -1;

    if (strcmp(fs->name, "pipefs") == 0)
        pipefs = (PipeFS *)fs;

    if (strcmp(fs->name, "sockfs") == 0)
        sockfs = (SockFS *)fs;

    char copy[PATH_MAX];
    strcpy(copy, mount_point);
    const char *filename = basename(copy);
    const char *parentname = dirname(copy);

    bool is_root = strcmp(mount_point, "/") == 0;

    DENTRY *dentry;

    if (is_root) {
        dentry = AllocDentry(0, "/");
    } else {
        dentry = GetDentry(mount_point);
    }

    if (!dentry)
        return -ENOENT;

    if (!is_root && (dentry->inode->mode & S_IFMT) != S_IFDIR)
        return -ENOTDIR;

    fs->root_dentry = dentry;
    fs->Mount(driver);

    if (!dentry->inode)
        return -1;

    if (is_root)
        root_dentry = dentry;

    dentry->owner = fs;

    return 0;
}

int DuplicateFile(Filetable *filetable, int oldfd)
{
    FILE *file = filetable->Get(oldfd);

    if (!file)
        return -EBADF;

    return filetable->Add(file);
}

int DuplicateFile(Filetable *filetable, int oldfd, int newfd)
{
    FILE *file = filetable->Get(oldfd);

    if (!file)
        return -EBADF;

    // Close previous file
    if (filetable->Get(newfd))
        Close(filetable, newfd);

    return filetable->Set(newfd, file);
}

int Fcntl(Filetable *filetable, int fd, int cmd, void *arg)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    if (cmd == F_DUPFD) {
        return filetable->Add(file, (int)arg);
    } else if (cmd == F_SETFD) {
        return 0;
    } else if (cmd == F_GETFD) {
        return 0;
    }

    return -1;
}

int Getdents(Filetable *filetable, int fd, dirent *dirp, unsigned int count)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    GetChildren(file->dentry);

    char *buf = (char *)dirp;
    unsigned int total = 0;
    int num_files = file->dentry->children.Count();

    if (file->pos >= num_files)
        return 0;

    for (int i = file->pos; i < num_files; i++) {
        dirp = (dirent *)&buf[total];
        DENTRY *dentry = file->dentry->children[i];

        if (dentry->inode) {
            int type = 0;

            switch (dentry->inode->mode & S_IFMT) {
            case S_IFIFO:
                type = DT_FIFO;
                break;
            case S_IFCHR:
                type = DT_CHR;
                break;
            case S_IFDIR:
                type = DT_DIR;
                break;
            case S_IFBLK:
                type = DT_BLK;
                break;
            case S_IFREG:
                type = DT_REG;
                break;
            case S_IFLNK:
                type = DT_LNK;
                break;
            case S_IFSOCK:
                type = DT_SOCK;
                break;
            default:
                type = DT_UNKNOWN;
                break;
            }

            const char *name = dentry->name;
            int name_len = strlen(name) + 1;
            int dirp_len = sizeof(dirent) - sizeof(dirp->d_name) + name_len;

            if (total + dirp_len >= count)
                break;

            dirp->d_ino = dentry->inode->ino ? dentry->inode->ino : -1; // TODO
            dirp->d_type = type;
            dirp->d_reclen = dirp_len;
            strcpy(dirp->d_name, name);

            total += dirp_len;
            file->pos += 1;
        }
    }

    return total;
}

int StatDentry(DENTRY *dentry, stat *st)
{
    if (!dentry || !dentry->inode)
        return -ENOENT;

    INODE *inode = dentry->inode;
    memset(st, 0, sizeof(stat));

    st->st_dev = 1;
    st->st_ino = inode->ino ? inode->ino : -1; // TODO
    st->st_mode = inode->mode;
    st->st_nlink = 1;
    st->st_uid = inode->uid;
    st->st_gid = inode->gid;
    st->st_rdev = inode->dev;
    st->st_size = inode->size;
    st->st_blksize = 512;
    st->st_blocks = 1;
    st->st_atime = inode->atime;
    st->st_mtime = inode->mtime;
    st->st_ctime = inode->ctime;
    return 0;
}

int Stat(PROCESS *process, const char *path, stat *st)
{
    DENTRY *dentry = GetDentry(process, path);

    if (PTR_ERR(dentry))
        return (int)dentry;

    return StatDentry(dentry, st);
}

int Fstat(Filetable *filetable, int fd, stat *st)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    file->lock.Aquire();
    int ret = StatDentry(file->dentry, st);
    file->lock.Release();
    return ret;
}

int Ioctl(Filetable *filetable, int fd, int request, unsigned int arg)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    file->lock.Aquire();

    DENTRY *dentry = file->dentry;
    INODE *inode = dentry->inode;

    Driver *driver = DriverManager::GetDriver(inode->dev);

    if (!driver) {
        file->lock.Release();
        return -ENOTTY;
    }

    int ret = driver->Ioctl(file, request, arg);
    file->lock.Release();
    return ret;
}

int Open(PROCESS *process, const char *path, int flags, mode_t mode)
{
    if (!path)
        return -EINVAL;

    DENTRY *dentry = GetDentry(process, path);

    if (PTR_ERR(dentry)) {
        if ((flags & O_CREAT) && (int)dentry == -ENOENT) {
            char copy[PATH_MAX];
            strcpy(copy, path);
            const char *filename = basename(copy);
            const char *parentname = dirname(copy);

            DENTRY *parent = GetDentry(process, parentname);

            if (PTR_ERR(parent))
                return (int)parent;

            mode_t create_mode = (mode & ~S_IFMT) | S_IFREG;
            int err = parent->owner->Create(parent, filename, create_mode);

            if (err)
                return err;

            dentry = GetDentry(process, path);

            if (PTR_ERR(dentry))
                return (int)dentry;
        } else {
            return (int)dentry;
        }
    }

    FILE *file = new FILE(dentry);
    file->lock.Aquire();
    file->flags = flags;
    int fd = process->filetable->Add(file);

    if (fd < 0) {
        file->lock.Release();
        return fd;
    }

    dentry->owner->Open(file);
    file->lock.Release();
    return fd;
}

int Close(Filetable *filetable, int fd)
{
    int ret = 0;
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    file->lock.Aquire();

    if ((ret = file->dentry->owner->Close(file))) {
        file->lock.Release();
        return ret;
    }

    filetable->Remove(fd);
    file->lock.Release();

    // delete file; // TODO
    return ret;
}

size_t Read(Filetable *filetable, int fd, char *dst, size_t size)
{
    FILE *file = filetable->Get(fd);

    if (!IsValidInode(file))
        return -ENOENT;

    file->lock.Aquire();
    Driver *owner = file->dentry->owner;

    if (S_ISREG(file->dentry->inode->mode)) {
        size = min(size, (size_t)(file->dentry->inode->size - file->pos));

        if ((int)size <= 0) {
            file->lock.Release();
            return 0;
        }
    }

    int read = owner->Read(file, dst, size);

    if (read >= 0)
        file->pos += read;

    file->lock.Release();
    return read;
}

size_t Write(Filetable *filetable, int fd, const char *buf, size_t size)
{
    FILE *file = filetable->Get(fd);

    if (!IsValidInode(file))
        return -ENOENT;

    file->lock.Aquire();

    fpos_t prev_pos = file->pos;

    if (file->flags & O_APPEND) {
        prev_pos = file->pos;
        file->pos = file->dentry->inode->size;
    }

    Driver *owner = file->dentry->owner;
    int written = owner->Write(file, buf, size);

    if (file->flags & O_APPEND)
        file->pos = prev_pos;

    if (written < 0) {
        file->lock.Release();
        return written;
    }

    file->pos += written;
    file->lock.Release();
    return written;
}

off_t Seek(Filetable *filetable, int fd, off_t offset, int whence)
{
    FILE *file = filetable->Get(fd);

    if (!IsValidInode(file))
        return -ENOENT;

    file->lock.Aquire();
    INODE *inode = file->dentry->inode;

    switch (whence) {
    case SEEK_SET:
        file->pos = offset;
        break;

    case SEEK_CUR:
        file->pos += offset;
        break;

    case SEEK_END:
        file->pos = inode->size + offset;
        break;

    default:
        file->lock.Release();
        return -EINVAL;
    }

    file->lock.Release();
    return 0;
}

int CreatePipeDentry(DENTRY *&dentry, int flags)
{
    int ret = 0;

    if ((ret = pipefs->Create(dentry, flags)))
        return ret;

    AddDentry(pipefs->root_dentry, dentry);
    return 0;
}

int CreatePipes(Filetable *filetable, int pipefd[2], int flags)
{
    int ret = 0;
    DENTRY *dentry;

    if ((ret = CreatePipeDentry(dentry, flags)))
        return ret;

    FILE *read = new FILE(dentry);
    FILE *write = new FILE(dentry);

    pipefd[0] = filetable->Add(read);
    pipefd[1] = filetable->Add(write);

    if (pipefd[0] < 0 || pipefd[1] < 0) {
        if (pipefd[0] < 0)
            filetable->Remove(pipefd[0]);

        if (pipefd[1] < 0)
            filetable->Remove(pipefd[1]);

        delete read;
        delete write;
        return -1;
    }

    return 0;
}

int CreateSocket(Filetable *filetable, int domain, int type, int protocol)
{
    int ret = 0;

    DENTRY *dentry;

    if ((ret = sockfs->Create(domain, type, protocol, dentry)))
        return ret;

    FILE *file = new FILE(dentry);
    int fd = filetable->Add(file);

    if (fd < 0)
        delete file;

    return fd;
}

int SocketAccept(Filetable *filetable, int fd, sockaddr *addr, socklen_t addrlen, int flags)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    DENTRY *dentry;
    int ret = 0;

    if ((ret = sockfs->Accept(file, addr, addrlen, flags, dentry)))
        return ret;

    FILE *client_file = new FILE(dentry);
    int client_fd = filetable->Add(client_file);

    if (client_fd < 0) {
        delete client_file;
        return client_fd;
    }

    return client_fd;
}

int SocketBind(Filetable *filetable, int fd, const sockaddr *addr, socklen_t addrlen)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    return sockfs->Bind(file, addr, addrlen);
}

int SocketConnect(Filetable *filetable, int fd, const sockaddr *addr, socklen_t addrlen)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    return sockfs->Connect(file, addr, addrlen);
}

int SocketListen(Filetable *filetable, int fd, int backlog)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    return sockfs->Listen(file, backlog);
}

int SocketRecv(Filetable *filetable, int fd, void *buf, size_t len, int flags)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    return sockfs->Recv(file, buf, len, flags);
}

int SocketRecvfrom(Filetable *filetable, int fd, void *buf, size_t len, int flags,
                   sockaddr *src_addr, socklen_t addrlen)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    return sockfs->Recvfrom(file, buf, len, flags, src_addr, addrlen);
}

int SocketSend(Filetable *filetable, int fd, const void *buf, size_t len, int flags)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    return sockfs->Send(file, buf, len, flags);
}

int SocketSendto(Filetable *filetable, int fd, const void *buf, size_t len, int flags,
                 const sockaddr *dest_addr, socklen_t addrlen)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    return sockfs->Sendto(file, buf, len, flags, dest_addr, addrlen);
}

int SocketShutdown(Filetable *filetable, int fd, int how)
{
    FILE *file = filetable->Get(fd);

    if (!file)
        return -EBADF;

    return sockfs->Shutdown(file, how);
}

uint32 ReadFile(const char *path, char *&buffer)
{
    DENTRY *dentry = GetDentry(0, path);

    if (PTR_ERR(dentry))
        return 0;

    int size = dentry->inode->size;
    buffer = new char[size];
    FILE file = FILE(dentry);
    return dentry->owner->Read(&file, buffer, size);
}

STATUS Init()
{
    root_dentry = 0;
    return STATUS_SUCCESS;
}
} // namespace VFS
