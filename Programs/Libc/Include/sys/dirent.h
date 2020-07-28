#pragma once
#include <sys/types.h>

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12

#define MAXNAMLEN 255 /* sizeof(struct dirent.d_name)-1 */

struct dirent
{
    ino_t d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
} __attribute__((packed));

typedef struct
{
    int dd_fd;  /* directory file */
    int dd_loc; /* position in buffer */
    int dd_seek;
    char *dd_buf; /* buffer */
    int dd_len;   /* buffer length */
    int dd_size;  /* amount of data in buffer */
} DIR;

//int getdents(unsigned int fd, struct dirent *dirp, unsigned int count);
int getdents(int fd, void *dp, int count);

#define __dirfd(dir) ((dir)->dd_fd)

/* --- redundant --- */

DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *__restrict, struct dirent *__restrict,
              struct dirent **__restrict);
void rewinddir(DIR *);
int closedir(DIR *);

/* internal prototype */
void _seekdir(DIR *dir, long offset);
DIR *_opendir(const char *);

#ifndef _POSIX_SOURCE
long telldir(DIR *);
void seekdir(DIR *, off_t loc);

int scandir(const char *__dir,
            struct dirent ***__namelist,
            int (*select)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **));

int alphasort(const struct dirent **__a, const struct dirent **__b);
#endif /* _POSIX_SOURCE */
