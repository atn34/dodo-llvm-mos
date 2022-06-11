extern unsigned char __bss_start[];
extern unsigned char __bss_end[];

#include <string.h>

void zero_bss() {
    memset(__bss_start, 0, __bss_end - __bss_start);
}
