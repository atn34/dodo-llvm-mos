extern unsigned char __bss_start[];
extern unsigned int __bss_size;

#include <string.h>

void _zero_bss() {
    memset(__bss_start, 0, __bss_size);
}
