[BITS 32]
[global _start]

extern grub_main
extern __DATA_END
extern __BSS_END

MB_ALIGN		equ  1<<0
MB_MEMINFO		equ  1<<1
MB_VIDINFO		equ  1<<2
MB_AOUT			equ  1<<16

MULTIBOOT_HEADER_MAGIC   	equ  0x1BADB002
MULTIBOOT_BOOTLOADER_MAGIC	equ  0x2BADB002
MULTIBOOT_HEADER_FLAGS   	equ  MB_ALIGN | MB_MEMINFO | MB_VIDINFO | MB_AOUT
CHECKSUM                 	equ  -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

STACK_SIZE equ 0x4000	;16k
KERNEL_STACK equ 0xFFB00000
PHYS_BASE equ 0x1000000
VIRT_BASE equ 0xC0000000
BOOTINFO_SIZE equ 96
VBEINFO_SIZE equ 256
VBEINFO_OFFSET equ 76

section .multiboot

multiboot_header:
	dd	MULTIBOOT_HEADER_MAGIC
	dd	MULTIBOOT_HEADER_FLAGS
	dd	CHECKSUM

	dd	PHYS_BASE
	dd	PHYS_BASE
	dd	__DATA_END - (VIRT_BASE - PHYS_BASE)
	dd	__BSS_END - (VIRT_BASE - PHYS_BASE)
	dd	_start
	
	dd	0
	dd	1024
	dd	768
	dd	32

_start:
	cmp eax, MULTIBOOT_BOOTLOADER_MAGIC
	jne .halt

	mov	esp, stack + STACK_SIZE

	call enable_cpu_features
	call copy_info
	call setup_paging

	and esp, 0xFFFFFFF0
	sub esp, 4
	push STACK_SIZE
	push stack
	push bootinfo
	call grub_main

.halt:
	cli
	hlt

enable_cpu_features:
.fpu:
	fninit
    fldcw [fcw]

.sse:
	mov eax, cr0
	and ax, 0xFFFB
	or ax, 0x22
	mov cr0, eax

	mov eax, cr4
	or eax, 0x600
	mov cr4, eax
	ret

copy_info:
	cld
	mov esi, ebx
	mov edi, bootinfo
	mov ecx, BOOTINFO_SIZE
	rep movsb

	mov esi, [bootinfo+VBEINFO_OFFSET]
	mov edi, vbeinfo
	mov [bootinfo+VBEINFO_OFFSET], edi
	mov ecx, VBEINFO_SIZE
	rep movsb
	ret

setup_paging:
	mov eax, page_dir
	mov ebx, page_tables
	mov esi, PHYS_BASE
	
	call setup_dir
	call map_self
	call map_stack
	call enable_paging
	ret

setup_dir:
	mov ecx, 1024
.loop:
	mov edx, ebx
	or edx, 3 			; present | writable
	mov [eax], edx

	cmp ecx, 256
	ja .next

	push ecx
	call setup_table
	pop ecx

.next:
	add eax, 4
	add ebx, 4096
	loop .loop
	ret

setup_table:
	mov ecx, 1024
	mov edi, ebx
.loop:
	mov edx, esi
	or edx, 3			; present | writable
	mov [edi], edx

	add esi, 4096
	add edi, 4
	loop .loop
	ret

map_self:
	mov eax, PHYS_BASE
	or eax, 3
	mov [page_tables + 4*4096], eax
	ret

map_stack:
	mov eax, stack
	mov edi, page_tables + ((KERNEL_STACK>>22) & 0x3FF)*4096 + ((KERNEL_STACK>>12) & 0x3FF)*4
	mov ecx, STACK_SIZE / 4096

.loop:
	or eax, 3
	mov [edi], eax
	add eax, 4096
	add edi, 4
	loop .loop
	ret

enable_paging:
	mov eax, page_dir
	mov cr3, eax

	add esp, KERNEL_STACK - stack

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret


fcw: dw 0x037F

;section .bss ; TODO
bootinfo: resb BOOTINFO_SIZE
vbeinfo: resb VBEINFO_SIZE

align 4096
stack: resb STACK_SIZE
page_dir: resb 4096
page_tables: resb 1024 * 4096
