#pragma once

/* XXX close on exec request; must match UF_EXCLOSE in user.h */
#define FD_CLOEXEC 1 /* posix */

/* fcntl(2) requests */
#define F_DUPFD         0  /* Duplicate fildes */
#define F_GETFD         1  /* Get fildes flags (close on exec) */
#define F_SETFD         2  /* Set fildes flags (close on exec) */
#define F_GETFL         3  /* Get file flags */
#define F_SETFL         4  /* Set file flags */
#define F_GETOWN        5  /* Get owner - for ASYNC */
#define F_SETOWN        6  /* Set owner - for ASYNC */
#define F_GETLK         7  /* Get record-locking information */
#define F_SETLK         8  /* Set or Clear a record-lock (Non-Blocking) */
#define F_SETLKW        9  /* Set or Clear a record-lock (Blocking) */
#define F_RGETLK        10 /* Test a remote lock to see if it is blocked */
#define F_RSETLK        11 /* Set or unlock a remote lock */
#define F_CNVT          12 /* Convert a fhandle to an open fd */
#define F_RSETLKW       13 /* Set or Clear remote record-lock(Blocking) */
#define F_DUPFD_CLOEXEC 14 /* As F_DUPFD, but set close-on-exec flag */

/* fcntl(2) flags (l_type field of flock structure) */
#define F_RDLCK   1 /* read lock */
#define F_WRLCK   2 /* write lock */
#define F_UNLCK   3 /* remove lock(s) */
#define F_UNLKSYS 4 /* remove remote locks for a given system */

/*
 * Flag values for open(2) and fcntl(2)
 * The kernel adds 1 to the open modes to turn it into some
 * combination of FREAD and FWRITE.
 */
#define O_RDONLY   0 /* +1 == FREAD */
#define O_WRONLY   1 /* +1 == FWRITE */
#define O_RDWR     2 /* +1 == FREAD|FWRITE */
#define O_APPEND   0x0008
#define O_CREAT    0x0200
#define O_TRUNC    0x0400
#define O_EXCL     0x0800
#define O_NONBLOCK 0x4000
#define O_NOCTTY   0x8000
#define O_SYNC     0x2000
