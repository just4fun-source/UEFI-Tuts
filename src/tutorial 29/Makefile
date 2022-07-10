# Makefile
CC        :=gcc
CFLAGS    :=-Wall -Werror -m64 -mabi=ms
UNAME_S   :=$(shell uname -s)

ifeq ($(UNAME_S),Darwin)
CC        :=x86_64-w64-mingw32-gcc
endif
ifeq ($(UNAME_S),Linux)
CC        :=x86_64-w64-mingw32-gcc
endif

all: BOOTX64.EFI loader.bin

%.o: %.c
	$(CC) $(CFLAGS) -I -ffreestanding -c $< -o $@

BOOTX64.EFI: efimain.o
	$(CC) $(CFLAGS) -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main $^ -o $@

loader.bin: loader/loader.c
	fasm gdt/gdt.asm gdt/gdt.bin
	$(CC) $(CFLAGS) -ffreestanding -Wl,--oformat=binary -e main -c $< -o loader/$@

clean:
	@echo "Cleaning files...."
	rm -f *.o *.EFI *.bin loader/*.bin gdt/*.bin
	@echo "Done."

.PHONY: clean
