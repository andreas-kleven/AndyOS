#pragma once

#define SYSCALL_IRQ		0x80
#define MAX_SYSCALLS	1024

#define SYSCALL_HALT				1
#define SYSCALL_PRINT				2
#define SYSCALL_COLOR				3
#define SYSCALL_GETTIME				4
#define SYSCALL_DRAW				5
#define SYSCALL_EXIT				6
#define SYSCALL_SLEEP				7
#define SYSCALL_TICKS				8
#define SYSCALL_GET_MOUSE_POS		9
#define SYSCALL_GET_MOUSE_BUTTONS	10
#define SYSCALL_ALLOC				0x10
#define SYSCALL_FREE				0x11