

section .text
extern handle_interrupt
global ignore_ireq

%macro handle_ireq 1
global handle_ireq%1
handle_ireq%1:
mov byte [intnum], byte (%1 + 0x20)
jmp int_bottom
%endmacro

%macro handle_excep 1
global handle_excep%1
handle_excep%1:
mov byte [intnum], byte %1
jmp int_bottom
%endmacro

handle_ireq 0x00
handle_ireq 0x01
handle_ireq 0x02
handle_ireq 0x03
handle_ireq 0x04
handle_ireq 0x05
handle_ireq 0x06
handle_ireq 0x07
handle_ireq 0x08
handle_ireq 0x09
handle_ireq 0x0A
handle_ireq 0x0B
handle_ireq 0x0C
handle_ireq 0x0D
handle_ireq 0x0E
handle_ireq 0x0F

handle_excep 0x00
handle_excep 0x02
handle_excep 0x01
handle_excep 0x03
handle_excep 0x04
handle_excep 0x05
handle_excep 0x06
handle_excep 0x07
handle_excep 0x08
handle_excep 0x09
handle_excep 0x0A
handle_excep 0x0B
handle_excep 0x0C
handle_excep 0x0D
handle_excep 0x0E
handle_excep 0x0F
handle_excep 0x10
handle_excep 0x11
handle_excep 0x12
handle_excep 0x13
handle_excep 0x14
handle_excep 0x15
handle_excep 0x16
handle_excep 0x17
handle_excep 0x18
handle_excep 0x19
handle_excep 0x1A
handle_excep 0x1B
handle_excep 0x1C
handle_excep 0x1D
handle_excep 0x1E
handle_excep 0x1F

int_bottom:

pusha
push ds
push es
push fs
push gs


movzx eax, byte [intnum]
push eax
call handle_interrupt
add esp, 4

pop gs
pop fs
pop es
pop ds
popa

ignore_ireq:
iret

section .data
intnum: db 0
