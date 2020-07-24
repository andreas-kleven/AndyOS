#pragma once
#include "types.h"
#include "list.h"
#include "file.h"
#include "stdio.h"
#include "filesystem.h"
#include "Process/scheduler.h"
#include "Net/address.h"

class FileSystem;
class BlockDriver;

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
	INODE *AllocInode(DENTRY *dentry);
	DENTRY *AllocDentry(DENTRY *parent, const char *name);
	DENTRY *GetDentry(Path path);
	void AddDentry(DENTRY *parent, DENTRY *child);
	int Mount(BlockDriver *driver, FileSystem *fs, const char *mount_point);

	int DuplicateFile(Filetable &filetable, int oldfd);
	int DuplicateFile(Filetable &filetable, int oldfd, int newfd);
	int Fcntl(Filetable &filetable, int fd, int cmd, void *arg);
	int Getdents(Filetable &filetable, int fd, dirent *dirp, unsigned int count);

	int Open(Filetable &filetable, const char *filename);
	int Close(Filetable &filetable, int fd);
	size_t Read(Filetable &filetable, int fd, char *buf, size_t size);
	size_t Write(Filetable &filetable, int fd, const char *buf, size_t size);
	off_t Seek(Filetable &filetable, int fd, off_t offset, int whence);
	int CreatePipeDentry(DENTRY *&dentry, int flags);
	int CreatePipes(Filetable &filetable, int pipefd[2], int flags);

	int CreateSocket(Filetable &filetable, int domain, int type, int protocol);
	int SocketAccept(Filetable &filetable, int fd, sockaddr *addr, socklen_t addrlen, int flags);
	int SocketBind(Filetable &filetable, int fd, const sockaddr *addr, socklen_t addrlen);
	int SocketConnect(Filetable &filetable, int fd, const sockaddr *addr, socklen_t addrlen);
	int SocketListen(Filetable &filetable, int fd, int backlog);
	int SocketRecv(Filetable &filetable, int fd, void *buf, size_t len, int flags);
	int SocketRecvfrom(Filetable &filetable, int fd, void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen);
	int SocketSend(Filetable &filetable, int fd, const void *buf, size_t len, int flags);
	int SocketSendto(Filetable &filetable, int fd, const void *buf, size_t len, int flags, const sockaddr *dest_addr, socklen_t addrlen);
	int SocketShutdown(Filetable &filetable, int fd, int how);

	uint32 ReadFile(const char *filename, char *&buffer);
	STATUS Init();
}; // namespace VFS