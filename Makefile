CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size

# -fno-tree-loop-distribute-patterns keeps GCC from turning the reset handler's
# copy/zero loops into memcpy/memset calls, which do not exist without a C library
CFLAGS  := -mcpu=cortex-m3 -mthumb -Os -g -std=c11 -Wall -Wextra \
           -ffreestanding -ffunction-sections -fdata-sections \
           -fno-tree-loop-distribute-patterns
LDFLAGS := -nostdlib -T link.ld -Wl,--gc-sections

BUILD := build
ELF   := $(BUILD)/fw-demo.elf
BIN   := $(BUILD)/fw-demo.bin
OBJS  := $(BUILD)/startup.o $(BUILD)/main.o

QEMU := qemu-system-arm -cpu cortex-m3 -machine lm3s6965evb -nographic \
        -semihosting-config enable=on,target=native -kernel

.PHONY: all run size clean

all: $(ELF) $(BIN)

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: src/%.c src/semihosting.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJS) link.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

run: $(ELF)
	$(QEMU) $<

size: $(ELF)
	$(SIZE) $<

clean:
	rm -rf $(BUILD)
