global rand:function (rand.end - rand)
global srand:function (srand.end - srand)
global div:function (div.end - div)
global ldiv:function (div.end - div)
global lldiv:function (lldiv.end - lldiv)

rand_seed: dd 1
rand:
push edx
; next = next * 1103515245 + 12345
mov eax, 1103515245
mul dword [rand_seed]
add eax, 12345
mov dword [rand_seed], eax

shr eax, byte 16
mov ecx, 32768
xor edx, edx
div ecx
mov eax, edx

pop edx
ret
.end:

; +4 seed
srand:
mov eax, dword [esp + 4]
mov dword [rand_seed], eax
ret
.end:

; +16 denom
; +12 number
; + 8 return value ptr
ldiv:
div:
push ebp
mov ebp, esp

mov ecx, dword [ebp + 8]
xor edx, edx
mov eax, dword [ebp + 12]
div dword [ebp + 16]
mov dword [ecx], eax
mov dword [ecx + 4], edx

pop ebp
ret 4
.end:

; +20 denom
; +12 number
; + 8 return value ptr
unsupported: db "lldiv() is unsupported!", 10, 0
extern printk
lldiv:
push ebp
mov ebp, esp

push unsupported
call printk
add esp, byte 4

pop ebp
ret
.end:

