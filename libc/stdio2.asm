section .text
global putchar
putchar:
mov eax, 0x00
movzx ecx, byte [esp + 4]
int 0xa0
ret

global puts
puts:
mov eax, 0x01
mov ecx, dword [esp + 4]
int 0xa0
ret