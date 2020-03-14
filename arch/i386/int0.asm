
section .text
global enable_interrupts
global disable_interrupts
global out_of_bounds
global vmm_enable

enable_interrupts:
sti
ret

disable_interrupts:
cli
ret

vmm_enable:
mov eax, cr0
or eax, 0x80000000
mov cr0, eax
ret

; +4 page
vmm_flush_page:
invlpg [esp + 4]
ret

out_of_bounds:
int 5
ret