
CC=i686-elf-gcc
CPP=i686-elf-g++
AR=i686-elf-ar
AS=nasm

FSHARED=-Wall -Wextra -fno-leading-underscore
CFLAGS=$(FSHARED) -std=gnu11
CPPFLAGS=$(FSHARED) -std=gnu++11 -fno-exceptions -fno-rtti -fno-use-cxa-atexit
ASFLAGS=-felf32

objects_asm=$(patsubst %.asm,%.o,$(wildcard *.asm))
objects_c  =$(patsubst %.c,%.o,$(wildcard *.c))
objects_cpp=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

objects=$(objects_c) $(objects_cpp) $(objects_asm)

libc.o: $(objects)
	$(AR) rcs $@ $(objects)
%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) -o $@ $<

clean:
	rm -f libc.o $(objects)
.PHONY: clean