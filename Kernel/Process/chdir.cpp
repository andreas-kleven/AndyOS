#include <FS/vfs.h>
#include <Process/process.h>
#include <Process/scheduler.h>
#include <errno.h>

namespace ProcessManager {
int ChdirDentry(PROCESS *process, DENTRY *dentry)
{
    if (!dentry)
        return -ENOENT;

    if (PTR_ERR(dentry))
        return (int)dentry;

    if ((dentry->inode->mode & S_IFMT) != S_IFDIR)
        return -ENOTDIR;

    process->pwd->refs -= 1;
    process->pwd = dentry;
    process->pwd->refs += 1;
    return 0;
}

int Chdir(PROCESS *process, const char *path)
{
    Scheduler::Disable();
    DENTRY *dentry = VFS::GetDentry(process, path);
    int ret = ChdirDentry(process, dentry);
    Scheduler::Enable();
    return ret;
}

int Fchdir(PROCESS *process, int fd)
{
    Scheduler::Disable();
    FILE *file = process->filetable->Get(fd);

    if (!file) {
        Scheduler::Enable();
        return -EBADF;
    }

    int ret = ChdirDentry(process, file->dentry);
    Scheduler::Enable();
    return ret;
}

}; // namespace ProcessManager
