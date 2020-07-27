#pragma once
#include <types.h>
#include <list.h>
#include <stdio.h>
#include <stat.h>
#include <FS/file.h>
#include <FS/filesystem.h>
#include <Process/scheduler.h>
#include <Net/address.h>

class FileSystem;
class BlockDriver;

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
	int StatDentry(DENTRY *dentry, stat *st);
	int Stat(const char *filename, stat *st);
	int Fstat(Filetable &filetable, int fd, stat *st);

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