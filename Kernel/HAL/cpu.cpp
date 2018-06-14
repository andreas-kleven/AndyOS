#include "cpu.h"

STATUS CPU::Init()
{
	if (!EnableFPU())
		return STATUS_FAILED;

	if (!EnableSSE())
		return STATUS_FAILED;

	return STATUS_SUCCESS;
}

STATUS CPU::EnableFPU()
{
	asm volatile(
		"mov %cr4, %eax\n"
		"or $0x200, %eax\n"
		"mov %eax, %cr4");

	return STATUS_SUCCESS;
}

STATUS CPU::EnableSSE()
{
	asm volatile(
		"mov %cr0, %eax\n"
		"and $0xFFFB, %ax\n"
		"or $0x2, %ax\n"
		"mov %eax, %cr0\n"

		"mov %cr4, %eax\n"
		"or $0x600, %eax\n"
		"mov %eax, %cr4");

	return STATUS_SUCCESS;
}