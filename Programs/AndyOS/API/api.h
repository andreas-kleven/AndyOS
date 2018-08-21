#pragma once
#include "syscalls.h"
#include "sys/types.h"
#include "keycodes.h"

int open(const char* filename, int flags);
int close(int fd);
size_t read(int fd, char* buf, size_t size);
size_t write(int fd, const char* buf, size_t size);
int pipe(int pipefd[2]);

void halt();
void print(const char* msg);
void color(uint32 color);
void draw(uint32* framebuffer);

void gettime(int& hours, int& minutes, int& seconds);
uint32 get_ticks();

void exit(int code);
void exit_thread(int code);
void sleep(uint32 ticks);

bool get_last_key(KEYCODE& code, bool& pressed);

uint32* alloc(uint32 blocks);
void free(void* ptr, uint32 blocks);
void alloc_shared(int proc_id, void*& addr1, void*& addr2, uint32 blocks);

int read_file(char*& buffer, char* filename);
int create_process(char* filename);

void debug_reset();
void debug_print(char* str, ...);