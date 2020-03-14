ENTRY(_start)

SECTIONS {
	. = 1M;
	__kernel_start = .;
	.text BLOCK(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text)
	}
	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}
	.data BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}
	.bss BLOCK(4K) : ALIGN(4K) {
		*(COMMON)
		*(.bss)
	}
	.heap BLOCK(4K) : ALIGN(4K) {
		*(.heap)
	}
	. = ALIGN(4K);
	__kernel_end = .;
}

