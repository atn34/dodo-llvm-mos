#!/bin/bash

echo $((0x7800 - 0x$(${LLVM_MOS_DIR:-$HOME/llvm-mos}/bin/llvm-nm fram.bin.elf | grep __data_end | awk '{print $1}')))
