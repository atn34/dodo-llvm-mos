MEMORY {
    ram  (rw) : ORIGIN = 0x200, LENGTH = 0x5600
    fram (rw) : ORIGIN = 0x5800, LENGTH = 0x2000
}

SECTIONS {
    .text 0x5800 : {
        . = . + 0x100;
        /* jmp _start */
        BYTE(0x4c)
        SHORT(_start)
        *(.text .text.*)
    } >fram
    .rodata : {
        *(.rodata .rodata.*)
    } >fram
    .data : {
        *(.data .data.*)
    } >fram
    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss .bss.* COMMON)
        __bss_end = .;
    }
    .noinit (NOLOAD) : {
        *(.noinit .noinit.*)
        __heap_start = .;
    }
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
