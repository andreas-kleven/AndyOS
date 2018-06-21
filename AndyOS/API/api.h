#pragma once
#include "definitions.h"
#include "keycodes.h"

void halt();
void print(const char* msg);
void color(uint32 color);
void gettime(int& hours, int& minutes, int& seconds);
void draw(uint32* framebuffer);
void exit(int code);
void sleep(uint32 ticks);
uint32 get_ticks();
void get_mouse_pos(int& x, int& y);
void get_mouse_buttons(bool& left, bool& right, bool& middle);
bool get_key_down(KEYCODE key);
uint32* alloc(uint32 blocks);
void free(void* ptr, uint32 blocks);
int read_file(char** buffer, char* filename);

void debug_reset();
void debug_print(char* str, ...);