MEMORY {
    ram  (rw) : ORIGIN = 0x200, LENGTH = 0x5600
    fram (rw) : ORIGIN = 0x5800, LENGTH = 0x2000
}

SECTIONS {
    .text 0x5800 : {
        . = . + 0x100;
	_start = .;
        /* A mechanism for dynamically building an _init script. */
        *(SORT_BY_INIT_PRIORITY(.init.* .init))
	*(.call_main)
	*(.text .text.*)
    } >fram
    .rodata : {
        INCLUDE rodata-sections.ld
    } >fram
    .data : {
        INCLUDE data-sections.ld
    } >fram
    INCLUDE bss.ld
    INCLUDE noinit.ld
}

__rc0 = 0x2a;
INCLUDE imag-regs.ld
ASSERT(__rc0 == 0x2a, "Inconsistent zero page map.")
ASSERT(__rc31 == 0x49, "Inconsistent zero page map.")

/* Set initial soft stack address to just above last ram address. (It grows down.) */
__stack = ORIGIN(ram) + LENGTH(ram);

OUTPUT_FORMAT {
    FULL(fram)
}
