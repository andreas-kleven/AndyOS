#pragma once

typedef signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned uint32, STATUS;
typedef long long int64;
typedef unsigned long long uint64;

typedef long unsigned int size_t;
typedef long signed int ssize_t;
typedef int pid_t;
typedef long int off_t;
typedef long int intptr_t;
typedef int32 blksize_t;
typedef int32 blkcnt_t;
typedef int32 dev_t;
typedef uint64 ino_t;
typedef uint32 uid_t;
typedef uint32 gid_t;
typedef unsigned short nlink_t;
typedef uint64 time_t;
typedef uint32 mode_t;
typedef uint32 useconds_t;

#define NULL 0
#define DIV_CEIL(a, b) (((a) == 0) ? 0 : (1 + (((a)-1) / (b))))

#define STATUS_FAILED 0
#define STATUS_SUCCESS 1

#define LOW_16(x) (uint16)(x & 0xFFFF)
#define HIGH_16(x) (uint16)((x << 16) & 0xFFFF)

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12

struct dirent
{
    ino_t d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};
