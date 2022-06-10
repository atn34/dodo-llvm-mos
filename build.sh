#!/bin/bash

set -euo pipefail

# for some reason ld.ldd fails sometimes with -flto

for LTO_ARG in -flto -fno-lto ; do
    set -x
    if "$LLVM_MOS_DIR"/bin/clang \
        -L"$LLVM_MOS_DIR"/mos-platform/common/lib/ \
        -Os \
        -Wl,--gc-sections \
        -fdata-sections \
        -ffunction-sections \
        $LTO_ARG \
        -g \
        -isystem "$LLVM_MOS_DIR"/mos-platform/common/include \
        -mcpu=mos65c02 \
        -o fram.bin \
        *.s *.c ; then
        { set +x; } 2>/dev/null
        break;
    fi
    { set +x; } 2>/dev/null
    echo "Retrying without link time optimization"
done
