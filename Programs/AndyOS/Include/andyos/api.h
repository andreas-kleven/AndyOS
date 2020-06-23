#pragma once
#include <sys/types.h>
#include "keycodes.h"

void halt();
void print(const char* msg);
void color(uint32_t color);
void draw(uint32_t* framebuffer);

void gettime(int& hours, int& minutes, int& seconds);
uint32_t get_ticks();

void exit_thread(int code);

bool get_last_key(KEYCODE& code, bool& pressed);

//uint32_t* alloc(uint32_t blocks);
//void free(void* ptr, uint32_t blocks);
void alloc_shared(int proc_id, void*& addr1, void*& addr2, uint32_t blocks);

int read_file(char*& buffer, const char* filename);
int create_process(const char* filename);

void debug_reset();
void debug_print(const char* str, ...);