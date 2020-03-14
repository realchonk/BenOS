MAGIC equ 0x1badb002
FLAGS equ (1<<0) | (1<<1)
CHECK equ -(MAGIC + FLAGS)

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECK

section .bss
align 16
stack_bottom:
resb 16384
stack_top:

section .text
global _start:function (_start.end - _start)
global cpu_reset:function (cpu_reset.end - cpu_reset)
global abort:function (abort.end - abort)
extern kernel_main
extern _init
extern _fini
_start:
	mov esp, stack_top

	push eax
	push ebx
	call _init

	movzx ecx, byte [aborted]
	; xor dl, dl				; optional
	; mov byte [aborted], dl	; optional
	push ecx
	call kernel_main
	add esp, 4
	call _fini
.hang:
	cli
	hlt
	jmp .hang
.end:

abort:
cli
mov al, byte 0x02
.l1:
test al, byte 0x02
jnz .l2
in al, 0x64
jmp .l1
.l2:
mov al, 0xfe
out 0x64, al
hlt
.end:

cpu_reset:
cli
.w1:
in al, byte 0x64
test al, byte 0b10
jnz .w1

mov al, byte 0xd1
out 0x64, al

.w2:
in al, byte 0x64
test al, byte 0b10
jnz .w2

mov al, byte 0xfe
out 0x60, al
hlt
.end:

section .data
aborted: db 0

global heap
section .bss
heap:
resb 4 << 20