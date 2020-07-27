#pragma once
#include <types.h>

#define S_IFMT 0170000	 // bit mask for the file type bit field
#define S_IFSOCK 0140000 // socket
#define S_IFLNK 0120000	 // symbolic link
#define S_IFREG 0100000	 // regular file
#define S_IFBLK 0060000	 // block device
#define S_IFDIR 0040000	 // directory
#define S_IFCHR 0020000	 // character device
#define S_IFIFO 0010000	 // FIFO

#define S_ISUID 04000 // set - user - ID bit(see execve(2))
#define S_ISGID 02000 // set - group - ID bit(see below)
#define S_ISVTX 01000 // sticky bit(see below)

#define S_IRWXU 00700 // owner has read, write, and execute permission
#define S_IRUSR 00400 // owner has read permission
#define S_IWUSR 00200 // owner has write permission
#define S_IXUSR 00100 // owner has execute permission

#define S_IRWXG 00070 // group has read, write, and execute permission
#define S_IRGRP 00040 // group has read permission
#define S_IWGRP 00020 // group has write permission
#define S_IXGRP 00010 // group has execute permission

#define S_IRWXO 00007 // others(not in group) have read, write, and execute permission
#define S_IROTH 00004 // others have read permission
#define S_IWOTH 00002 // others have write permission
#define S_IXOTH 00001 // others have execute permission

#define S_ISBLK(m) (((m)&S_IFMT) == S_IFBLK)
#define S_ISCHR(m) (((m)&S_IFMT) == S_IFCHR)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#define S_ISFIFO(m) (((m)&S_IFMT) == S_IFIFO)
#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#define S_ISLNK(m) (((m)&S_IFMT) == S_IFLNK)
#define S_ISSOCK(m) (((m)&S_IFMT) == S_IFSOCK)

struct stat
{
	dev_t st_dev;
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	off_t st_size;
	time_t st_atime;
	long st_spare1;
	time_t st_mtime;
	long st_spare2;
	time_t st_ctime;
	long st_spare3;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	long st_spare4[2];
};
