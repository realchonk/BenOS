
section .text
global enable_interrupts
global disable_interrupts
global _ZN2mm13load_page_dirEPv
global _ZN2mm13enable_pagingEv

enable_interrupts:
sti
ret

disable_interrupts:
cli
ret

_ZN2mm13load_page_dirEPv:
push ebp
mov ebp, esp
mov eax, dword [esp + 8]


;extern printk
;push eax
;push format
;call printk
;add esp, 8

;and eax, 0xfffff000
mov eax, dword [esp + 8]
mov cr3, eax

mov esp, ebp
pop ebp
ret


_ZN2mm13enable_pagingEv:
mov eax, cr0
or eax, 0x80000000
mov cr0, eax
ret

format: db "load_page_dir(%p)", 10, 0

