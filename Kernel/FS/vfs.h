#pragma once
#include "types.h"
#include "Process/scheduler.h"
#include "file.h"
#include "stdio.h"

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

namespace VFS
{
	int DuplicateFile(int oldfd);
	int DuplicateFile(int oldfd, int newfd);

	int Open(const char *filename);
	int Close(int fd);
	size_t Read(int fd, char *buf, size_t size);
	size_t Write(int fd, const char *buf, size_t size);
	off_t Seek(int fd, off_t offset, int whence);
	int CreatePipes(int pipefd[2], int flags);

	uint32 ReadFile(const char *filename, char *&buffer);
	STATUS Init();
}; // namespace VFS