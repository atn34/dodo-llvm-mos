MEMORY {
    ram  (rw) : ORIGIN = 0x200, LENGTH = 0x5600
    fram (rw) : ORIGIN = 0x5800, LENGTH = 0x2000
}

SECTIONS {
    .text 0x5800 : {
        . = . + 0x100;
        /* lda #<__stack */
        BYTE(0xA9)
        BYTE(__stack % 256)
        /* sta __rc0 */
        BYTE(0x85)
        BYTE(__rc0)
        /* lda #>__stack */
        BYTE(0xA9)
        BYTE(__stack / 256)
        /* sta __rc0 + 1 */
        BYTE(0x85)
        BYTE(__rc0 + 1)
        /* lda #__rc0 */
        BYTE(0xA9)
        BYTE(__rc0)
        /* sta $00 */
        BYTE(0x85)
        BYTE(0x00)
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
    }
    __bss_size = SIZEOF(.bss);
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
