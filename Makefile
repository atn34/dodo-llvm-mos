.PHONY: all
all: fram.bin

LLVM_MOS_DIR?=

%.cpp.o: %.cpp
	$(LLVM_MOS_DIR)/bin/clang++ \
		$^ \
		-Oz \
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
		-Oz \
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
		-Oz \
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
	$(LLVM_MOS_DIR)/bin/llvm-objdump -d fram.bin.elf --print-imm-hex --demangle
