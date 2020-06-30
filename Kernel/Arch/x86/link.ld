ENTRY(_start)
SECTIONS
{
	. = 16M;
	__KERNEL_START = .;

	.multiboot BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
	}

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

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss)
	}

	__KERNEL_END = .; 
}