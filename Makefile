LLVM_MOS_DIR?=$(HOME)/llvm-mos
FLAGS ?= -flto -Oz -g -Wall -Wextra -Werror -mcpu=mos65c02

.PHONY: all
all: fram.bin

%.cpp.o: %.cpp
	$(LLVM_MOS_DIR)/bin/clang++ \
		$(FLAGS) \
		$^ \
		-c -std=c++11 -fdata-sections -ffunction-sections \
		-isystem $(LLVM_MOS_DIR)/mos-platform/common/include \
	       	-o $@

%.c.o: %.c
	$(LLVM_MOS_DIR)/bin/clang \
		$(FLAGS) \
		$^ \
		-c -std=c99 -fdata-sections -ffunction-sections \
		-isystem $(LLVM_MOS_DIR)/mos-platform/common/include \
	       	-o $@

%.s.o: %.s
	$(LLVM_MOS_DIR)/bin/clang \
		$(FLAGS) \
		$^ \
		-c -o $@

fram.bin: $(patsubst %,%.o,$(wildcard *.s)) $(patsubst %,%.o,$(wildcard *.c)) $(patsubst %,%.o,$(wildcard *.cpp))
	$(LLVM_MOS_DIR)/bin/clang++ \
		$(FLAGS) \
		$^ \
		-L $(LLVM_MOS_DIR)/mos-platform/common/lib \
		-Wl,--gc-sections -linit-stack -lzero-bss -nostartfiles \
		-o $@

.PHONY: clean
clean:
	rm -f *.o fram.*

.PHONY: disass 
disass: fram.bin
	@$(LLVM_MOS_DIR)/bin/llvm-objdump --source fram.bin.elf --print-imm-hex --demangle


.PHONY: symbols
symbols: fram.bin
	@$(LLVM_MOS_DIR)/bin/llvm-nm fram.bin.elf --numeric-sort --demangle | egrep '^0000(5[9abcdef]|6|7[01234567])'

.PHONY: free_space
free_space: fram.bin
	@./print_free_space.sh
