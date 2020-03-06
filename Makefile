ARCH=i386
ARCHDIR=arch/$(ARCH)

CC=i686-elf-gcc
CPP=i686-elf-g++
LD=i686-elf-ld -O3
AS=nasm

INCLUDES=-Iinclude -Iinclude/libc
CFLSHARED=-ffreestanding -Wall -Wextra -fstack-protector -fno-builtin -fno-leading-underscore -mfpmath=387
CFLAGS=-std=gnu11 $(CFLSHARED) $(INCLUDES)
CPPFLAGS=-std=gnu++11 -fno-exceptions -fno-rtti -fno-use-cxa-atexit $(CFLSHARED) $(INCLUDES)
LDFLAGS=-melf_i386 -nostdlib -L.
ASFLAGS=-felf32

CRT_BEGIN=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRT_END=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

objects_arch_c  =$(patsubst $(ARCHDIR)/%.c,$(ARCHDIR)/%.o,$(wildcard $(ARCHDIR)/*.c))
objects_arch_cpp=$(patsubst $(ARCHDIR)/%.cpp,$(ARCHDIR)/%.o,$(wildcard $(ARCHDIR)/*.cpp))
objects_arch_asm=$(patsubst $(ARCHDIR)/%.asm,$(ARCHDIR)/%.o,$(wildcard $(ARCHDIR)/*.asm))
objects_arch=$(objects_arch_c) $(objects_arch_cpp) $(objects_arch_asm)

objects_kernel_c  =$(patsubst kernel/%.c,kernel/%.o,$(wildcard kernel/*.c))
objects_kernel_cpp=$(patsubst kernel/%.cpp,kernel/%.o,$(wildcard kernel/*.cpp))
objects_kernel=$(objects_kernel_c) $(objects_kernel_cpp)

objects_lib_c  =$(patsubst lib/%.c,lib/%.o,$(wildcard lib/*.c))
objects_lib_cpp=$(patsubst lib/%.cpp,lib/%.o,$(wildcard lib/*.cpp))
objects_lib=$(objects_lib_c) $(objects_lib_cpp)

objects=crti.o $(objects_arch) $(objects_kernel) $(objects_lib) crtn.o

kernel.bin: $(objects)
	$(LD) -Tlinker.ld -o $@ $(LDFLAGS) $(CRT_BEGIN) $(objects) $(CRT_END) -lgcc

%.o: %.asm
	nasm -felf32 -o $@ $<

$(ARCHDIR)/%.o: $(ARCHDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
$(ARCHDIR)/%.o: $(ARCHDIR)/%.cpp
	$(CPP) -c -o $@ $< $(CPPFLAGS)
$(ARCHDIR)/%.o: $(ARCHDIR)/%.asm
	$(AS) -o $@ $< $(ASFLAGS)

kernel/%.o: kernel/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
kernel/%.o: kernel/%.cpp
	$(CPP) -c -o $@ $< $(CPPFLAGS)


lib/%.o: lib/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
lib/%.o: lib/%.cpp
	$(CPP) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -rf $(objects) iso/

run: kernel.bin
	qemu-system-i386 -kernel kernel.bin -m 64M # -no-reboot-d int,cpu_reset
runvbox: benos.iso
	virtualbox --startvm BenOS

benos.iso: kernel.bin grub.cfg
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso/
install: kernel.bin
	sudo cp $< /boot/benos.bin

device ?= "100101"
burn: benos.iso
	if [ "$(device)" = "100101" ]; then echo device is empty; exit 1; fi
	sudo dd bs=4M if=$< of=$(device) conv=fdatasync

.PHONY: clean run runvbox install burn