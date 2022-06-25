LLVM_MOS_DIR?=$(HOME)/llvm-mos/
OPT ?= -Oz

.PHONY: all
all: fram.bin

%.cpp.o: %.cpp
	$(LLVM_MOS_DIR)/bin/clang++ \
		$^ \
		$(OPT) \
		-c \
		-fdata-sections \
		-ffunction-sections \
		-flto \
		-g \
		-isystem $(LLVM_MOS_DIR)/mos-platform/common/include \
		-std=c++11 \
	       	-o $@

%.c.o: %.c
	$(LLVM_MOS_DIR)/bin/clang \
		$^ \
		$(OPT) \
		-c \
		-fdata-sections \
		-ffunction-sections \
		-flto \
		-g \
		-isystem $(LLVM_MOS_DIR)/mos-platform/common/include \
		-std=c99 \
	       	-o $@

%.s.o: %.s
	$(LLVM_MOS_DIR)/bin/clang \
		$^ \
		-c \
		-g \
		-mcpu=mos65c02 \
		-o $@

fram.bin: $(patsubst %,%.o,$(wildcard *.s)) $(patsubst %,%.o,$(wildcard *.c)) $(patsubst %,%.o,$(wildcard *.cpp))
	$(LLVM_MOS_DIR)/bin/clang++ \
		$^ \
		-L $(LLVM_MOS_DIR)/mos-platform/common/lib \
		$(OPT) \
		-Wl,--gc-sections \
		-flto \
		-g \
		-isystem $(LLVM_MOS_DIR)/mos-platform/common/include \
		-linit-stack \
		-lzero-bss \
		-mcpu=mos65c02 \
		-nostartfiles \
		-o $@

.PHONY: clean
clean:
	rm -f *.o fram.*

.PHONY: disass 
disass: fram.bin
	@$(LLVM_MOS_DIR)/bin/llvm-objdump -d fram.bin.elf --print-imm-hex --demangle


.PHONY: symbols
symbols: fram.bin
	@$(LLVM_MOS_DIR)/bin/llvm-nm fram.bin.elf --numeric-sort --demangle | egrep '^0000(5[9abcdef]|6|7[01234567])'

.PHONY: free_space
free_space: fram.bin
	@./print_free_space.sh
