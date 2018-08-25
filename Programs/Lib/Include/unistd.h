#pragma once
#include "sys/types.h"

typedef int ssize_t;

extern char** environ;

int open(const char* filename, int flags);
int close(int fd);
ssize_t read(int fd, char* buf, size_t size);
ssize_t write(int fd, const char* buf, size_t size);
int seek(int fd, long int offset, int origin);
int pipe(int pipefd[2]);

int dup(int oldfd);
int dup2(int oldfd, int newfd);

pid_t fork();
pid_t getpid();

int execl(char const *path, char const *arg, ...);
int execlp(char const *file, char const *arg, ...);
//int execle(char const *path, char const *arg, ..., char const *envp[]);
int execv(char const *path, char const *argv[]);
int execvp(char const *file, char const *argv[]);
int execve(char const *path, char const *argv[], char const *envp[]);