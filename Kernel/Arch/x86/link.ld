ENTRY(_start)
SECTIONS
{
	. = 0x1000000;
	__BOOT_START = .;

	.multiboot BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
	}

	__BOOT_END = .;
	__KERNEL_START = 0xC0000000;
	. += 0xC0000000-0x1000000;
	
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		__START_CTORS = .;
        *(SORT(.ctors*))
        __END_CTORS = .;

        __START_DTORS = .;
        *(SORT(.dtors*))
        __END_DTORS = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	__DATA_END = .; 

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss)
	}

	__BSS_END = .; 
	__KERNEL_END = .; 
}
