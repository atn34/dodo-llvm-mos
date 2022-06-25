#!/bin/bash

set -euo pipefail

# for some reason ld.ldd fails sometimes with -flto

for LTO_ARG in -flto -fno-lto ; do
    set -x
    if "$LLVM_MOS_DIR"/bin/clang \
	-Oz \
	-nostartfiles \
	-std=c++11 \
        $LTO_ARG \
        -L "$LLVM_MOS_DIR"/mos-platform/common/lib \
        -Wl,--gc-sections \
        -fdata-sections \
        -ffunction-sections \
        -g \
        -isystem "$LLVM_MOS_DIR"/mos-platform/common/include \
        -linit-stack \
        -lzero-bss \
        -mcpu=mos65c02 \
        -o fram.bin \
        *.s *.cpp ; then
        { set +x; } 2>/dev/null
        break;
    fi
    { set +x; } 2>/dev/null
    echo "Retrying without link time optimization"
done
