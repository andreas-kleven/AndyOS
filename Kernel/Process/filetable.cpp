#include "filetable.h"
#include "debug.h"

Filetable::Filetable(int reserved)
{
    this->reserved = reserved;

    for (int i = 0; i < reserved; i++)
        Add(0);
}

int Filetable::Add(FILE *file)
{
    for (int i = reserved; i < files.Count(); i++)
    {
        if (files[i] == 0)
            return Set(i, file);
    }

    int fd = files.Count();
    files.Add(file);

    if (file)
        file->dentry->refs += 1;

    return fd;
}

int Filetable::Remove(int fd)
{
    FILE *file = Get(fd);

    if (file == 0)
        return -1;

    Set(fd, 0);
    return 0;
}

FILE *Filetable::Get(int fd)
{
    if (fd < 0 || fd >= files.Count())
        return 0;

    return files[fd];
}

int Filetable::Set(int fd, FILE *file)
{
    if (fd < 0 || fd >= files.Count())
        return -1;

    if (files[fd] != file)
    {
        if (files[fd])
            files[fd]->dentry->refs -= 1;
        if (file)
            file->dentry->refs += 1;
    }

    files[fd] = file;
    return fd;
}

Filetable Filetable::Clone()
{
    Filetable clone;

    for (int i = 0; i < files.Count(); i++)
        clone.Set(i, new FILE(*files[i]));

    return clone;
}
