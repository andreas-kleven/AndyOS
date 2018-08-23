#pragma once
#include "sys/types.h"

typedef int ssize_t;

int open(const char* filename, int flags);
int close(int fd);
ssize_t read(int fd, char* buf, size_t size);
ssize_t write(int fd, const char* buf, size_t size);
int pipe(int pipefd[2]);

pid_t fork();
pid_t getpid();