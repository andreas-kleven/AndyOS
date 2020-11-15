#pragma once
#include <Arch/regs.h>
#include <irq.h>
#include <types.h>

#define MAX_INTERRUPTS 256
#define INTERRUPT      __attribute__((naked))

#define KERNEL_CS 0x08
#define KERNEL_SS 0x10
#define KERNEL_DS 0x10

#define USER_CS 0x1B
#define USER_SS 0x23
#define USER_DS 0x23

#define IDT_DESC_BIT16   0x06 // 00000110
#define IDT_DESC_BIT32   0x0E // 00001110
#define IDT_DESC_RING1   0x40 // 01000000
#define IDT_DESC_RING2   0x20 // 00100000
#define IDT_DESC_RING3   0x60 // 01100000
#define IDT_DESC_PRESENT 0x80 // 10000000

typedef void (*ISR)();

struct IDT_DESCRIPTOR
{
    uint16 low;
    uint16 sel;
    uint8 reserved;
    uint8 flags;
    uint16 high;
} __attribute__((packed));

struct IDT_REG
{
    uint16 limit;
    uint32 base;
} __attribute__((packed));

namespace IDT {

bool SetISR(uint32 i, ISR isr, int flags);
bool InstallIRQ(uint32 i, void (*handler)());
bool InstallIRQ(uint32 i, void (*handler)(REGS *));
bool Init();
bool Enable();

}; // namespace IDT
