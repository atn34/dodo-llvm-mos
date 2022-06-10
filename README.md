# dodo-llvm-mos

1. Download [llvm-mos](https://github.com/llvm-mos/llvm-mos-sdk#download)
1. `LLVM_MOS_DIR=/path/to/llvm-mos ./build.sh`

This builds tetris for the [dodo](https://github.com/peternoyes/dodo) using llvm.

# How does it work?

The dodo API uses the cc65 calling convention, so `api.s` provides a shim layer to convert the llvm-mos calling convention to cc65 for each symbol.

`link.ld` teaches the linker how to create a fram for dodo.
# TODO

1. CHECK_VERSION is not implemented for dodo
1. Clean this up following [the official advice on porting](https://llvm-mos.org/wiki/Porting) and try to get this upstreamed