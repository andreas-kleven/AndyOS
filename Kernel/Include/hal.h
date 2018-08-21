#pragma once
#include "types.h"

void pause();
void enable();
void disable();
void sys_halt();

uint8 inb(uint16 port);
uint16 inw(uint16 port);
uint32 inl(uint16 port);

void outb(uint16 port, uint8 val);
void outw(uint16 port, uint16 val);
void outl(uint16 port, uint32 val);