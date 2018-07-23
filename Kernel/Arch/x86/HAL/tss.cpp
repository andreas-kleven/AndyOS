#include "Arch/tss.h"
#include "Arch/gdt.h"
#include "Arch/idt.h"
#include "hal.h"
#include "string.h"

#define GDT_FLAG 0xE9

namespace TSS
{
	TSS_ENTRY tss;

	void Flush() 
	{
		asm volatile (
			"cli\n"
			"movw $0x2B, %ax\n"
			"ltr %ax");
	}

	void SetStack(uint32 kernelSS, uint32 kernelESP) {

		tss.ss0 = kernelSS;
		tss.esp0 = kernelESP;
	}

	STATUS Init(uint32 gdt_index, uint32 kernelESP)
	{
		uint32 base = (uint32)&tss;
		memset((void*)&tss, 0, sizeof(TSS_ENTRY));

		if (!GDT::SetDescriptor(gdt_index, base, base + sizeof(TSS_ENTRY), GDT_FLAG))
			return STATUS_FAILED;

		tss.ss0 = KERNEL_SS;
		tss.esp0 = kernelESP;
		tss.cs = 0x0B;
		tss.ss = 0x13;
		tss.es = 0x13;
		tss.ds = 0x13;
		tss.fs = 0x13;
		tss.gs = 0x13;

		Flush();

		return STATUS_SUCCESS;
	}
}