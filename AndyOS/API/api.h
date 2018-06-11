#pragma once
#include "definitions.h"

void halt();
void print(const char* msg);
void color(uint32 color);
void gettime(int& hours, int& minutes, int& seconds);
void draw(uint32* framebuffer);
void exit(int code);
void sleep(uint32 ticks);
uint32 ticks();
void get_mouse_pos(int& x, int& y);
void get_mouse_buttons(bool& left, bool& right, bool& middle);
uint32* alloc(uint32 blocks);
void free(void* ptr, uint32 blocks);