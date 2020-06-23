#pragma once

typedef long unsigned int size_t;
typedef int pid_t;
typedef long int off_t;
typedef long int intptr_t;

typedef signed char			int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned			uint32, STATUS;
typedef long long			int64;
typedef unsigned long long	uint64;

#define STATUS_FAILED 0
#define STATUS_SUCCESS 1

#define LOW_16(x) (uint16)(x & 0xFFFF)
#define HIGH_16(x) (uint16)((x << 16) & 0xFFFF)
