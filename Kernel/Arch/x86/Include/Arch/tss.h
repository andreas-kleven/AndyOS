#pragma once
#include <types.h>

struct TSS_ENTRY
{
	uint32 prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
	uint32 esp0;       // The stack pointer to load when we change to kernel mode.
	uint32 ss0;        // The stack segment to load when we change to kernel mode.
	uint32 esp1;       // everything below here is unusued now.. 
	uint32 ss1;
	uint32 esp2;
	uint32 ss2;
	uint32 cr3;
	uint32 eip;
	uint32 eflags;
	uint32 eax;
	uint32 ecx;
	uint32 edx;
	uint32 ebx;
	uint32 esp;
	uint32 ebp;
	uint32 esi;
	uint32 edi;
	uint32 es;
	uint32 cs;
	uint32 ss;
	uint32 ds;
	uint32 fs;
	uint32 gs;
	uint32 ldt;
	uint16 trap;
	uint16 iomap_base;
} __attribute__((packed));

namespace TSS
{
	void Flush();
	void SetStack(uint32 kernelSS, uint32 kernelESP);
	STATUS Init(uint32 gdt_index, uint32 kernelESP);
};

