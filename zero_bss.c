extern unsigned char __bss_start[];
extern unsigned char __bss_end[];

void zero_bss() {
    for (unsigned char* iter = __bss_start; iter < __bss_end; ++iter) {
        *iter = 0;
    }
}
