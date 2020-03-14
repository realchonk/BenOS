# BenOS
A small operating system I'm currently developing

# Main Features
- Serial Connection
- Virtual Memory (paging, no swapping)
- Multitasking
- VGA (currently only 320x200x8)
- VGA double buffering
- Keyboard & Mouse (only PS/2, no USB)
- 3 VGA ASCII text sizes (16x16, 12x12, 8x8)
- Stack-Smashing-Protector (gcc -fstack-protector)
- Very basic exception handling with sys_excep(const char* msg);

# User Interface
- v0.1
  - a simple x86 text-mode based editor
  - requires keyboard
  - supports mouse
  - controlls:
    - select color: ALT + (0 to 9, A to F)
    - select background color: additionally hold shift
    - CTRL + DEL for reboot (doesn't work on all machines)
- v0.2+
  - a simple VGA-based paint program
  - requires keyboard & mouse
  - controlls:
    - same as with v0.1 except you don't have to hold ALT
    - ESC to clear screen

# Building
First you need a compiler to build the project.<br>
Since I currently don't have a os-specific compiler you have to build it on your own.<br>
You can find the Tools as a release.<br>
Dependencies:
- make
- nasm
- xorriso
- and of course the gcc toolchain, which you built already

# Burning on a flash drive/CDROM
DISCLAIMER: Please be carefull you can accidently format a different drive.<br>
DISCLAIMER: This is only tested on Linux. It may or may not work with WSL.<br>
For burning the .iso you should write:<br>
<code>make burn device=/dev/(insert your drive here)</code><br>
This asks you to enter you admin user/password.<br>
If you don't trust just look at the Makefile<br>
You can find all drives with <code>lsblk</code><br>
example: <code>make burn device=/dev/sdc</code>

# TODO (priority order: top to down)
- Add filesystem support
- Add more system calls
- Add more video modes
- Add USB support
- Implement a shell
- Implement a BASIC interpreter
- Create OS-specific compiler

# Sources
- lowlevel.eu
- osdev.org
- wyoos.org (Viktor Engelmann)
- wikipedia.org
