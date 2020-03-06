section .text
global memcpy:function (memcpy.end - memcpy)
global memset:function (memset.end - memset)
global strlen:function (strlen.end - strlen)
global memcmp:function (memcmp.end - memcmp)
global strcpy:function (strcpy.end - strcpy)
global strncpy:function (strncpy.end - strncpy)
global strcat:function (strcat.end - strcat)
global strncat:function (strncat.end - strncat)
global strcmp:function (strcmp.end - strcmp)
global strncmp:function (strncmp.end - strncmp)
global memchr:function (memchr.end - memchr)
global strchr:function (strchr.end - strchr)
global strnlen:function (strnlen.end - strnlen)
global strlwr:function	(strlwr.end - strlwr)
global strupr:function	(strupr.end - strupr)
global strrvs:function	(strrvs.end - strrvs)
global strnlwr:function	(strnlwr.end - strnlwr)
global strnupr:function (strnupr.end - strnupr)
global strnrvs:function	(strnrvs.end - strnrvs)

; + 8 dest
; +12 src
; +16 num
memcpy:
push ebp
mov ebp, esp
push esi
push edi

xor ecx, ecx
mov edi, dword [ebp + 8]	; dest
mov esi, dword [ebp + 12]	; source
mov edx, dword [ebp + 16]	; num
.l1:
cmp edx, ecx
jl .l2

mov al, byte [esi + ecx]
mov byte [edi + ecx], al

inc ecx
jmp .l1
.l2:
mov eax, dword [ebp + 8]
pop edi
pop esi
pop ebp
ret
.end:

; + 8 dest
; +12 val
; +16 num
memset:
push ebp
mov ebp, esp
push ebx

mov eax, dword [ebp +  8]
mov edx, dword [ebp + 16]
mov bl, byte [ebp + 12]
xor ecx, ecx
.l1:
cmp ecx, edx
jge .l2

mov byte [eax + ecx], bl
inc ecx
jmp .l1

.l2:
pop ebx
pop ebp
ret
.end:

; +8 str
strlen:
push ebp
mov ebp, esp

xor eax, eax
mov edx, dword [ebp + 8]

.l1:
mov cl, byte [edx + eax]
cmp cl, byte 0
jz .l2

inc eax
jmp .l1
.l2:

pop ebp
ret
.end:

; + 8 void* p1
; +12 void* p2
; +16 size_t num
memcmp:
push ebp
mov ebp, esp
push edi
push esi

xor ecx, ecx
mov edi, dword [ebp +  8]
mov esi, dword [ebp + 12]
mov edx, dword [ebp + 16]
.l1:
cmp edx, ecx
jl .l2

mov al, byte [edi + ecx]
cmp al, byte [esi + ecx]
jg .l3
jl .l4
inc ecx
jmp .l1
.l3:
mov eax, 1
jmp .l5
.l4:
mov eax, -1
jmp .l5

.l2:
xor eax, eax
.l5:
pop esi
pop edi
pop ebp
ret
.end:

; + 8 dest
; +12 src
strcpy:
push ebp
mov ebp, esp
push esi

mov eax, dword [ebp +  8]
mov esi, dword [ebp + 12]
xor ecx, ecx
.l1:
mov dl, byte [esi + ecx]
cmp dl, byte 0
jz .l2
mov byte [eax + ecx], dl
inc ecx
jmp .l1

.l2:
mov byte [eax + ecx], byte 0
pop esi
pop ebp
ret
.end:

; + 8 dest
; +12 src
; +16 num
strncpy:
push ebp
mov ebp, esp
push esi
push ebx

mov eax, dword [ebp +  8]
mov esi, dword [ebp + 12]
mov ebx, dword [ebp + 16]
xor ecx, ecx
.l1:
cmp ecx, ebx
jge .l2
mov dl, byte [esi + ecx]
cmp dl, byte 0
jz .l2

mov byte [eax + ecx], dl
inc ecx
jmp .l1

.l2:
mov byte [eax + ecx], byte 0
pop ebx
pop esi
pop ebp
ret
.end:

; + 8 dest
; +12 src
strcat:
push ebp
mov ebp, esp

push dword [ebp + 8]
call strlen
add esp, byte 4

add eax, dword [ebp + 8]
add esp, byte 4
push dword [ebp + 12]
push eax
call strcpy
add esp, byte 12

mov eax, dword [ebp + 8]
pop ebp
ret
.end:

; + 8 dest
; +12 src
; +16 num
strncat:
push ebp
mov ebp, esp

push dword [ebp + 8]
call strlen
add esp, byte 4

add eax, dword [ebp + 8]
push dword [ebp + 16]
push dword [ebp + 12]
push eax
call strncpy
add esp, byte 12

mov eax, dword [ebp + 8]
pop ebp
ret
.end:

; + 8 s1
; +12 s2
strcmp:
push ebp
mov ebp, esp
push edi
push esi

mov edi, dword [ebp +  8]
mov esi, dword [ebp + 12]
xor ecx, ecx
.l1:
mov al, byte [edi + ecx]
mov dl, byte [esi + ecx]
cmp al, dl
jg .greater
jl .less

cmp al, byte 0
jz .equal

inc ecx
jmp .l1

.greater:
mov eax, 1
jmp .return
.less:
mov eax, -1
jmp .return
.equal:
xor eax, eax

.return:
pop esi
pop edi
pop ebp
ret
.end:

; + 8 s1
; +12 s2
; +16 num
strncmp:
push ebp
mov ebp, esp
push edi
push esi

mov edi, dword [ebp +  8]
mov esi, dword [ebp + 12]
mov edx, dword [ebp + 16]
xor ecx, ecx
.l1:
cmp edx, ecx
jl .equal
mov al, byte [edi + ecx]
cmp al, byte [esi + ecx]
jg .greater
jl .less
cmp al, byte 0
jz .equal

inc ecx
jmp .l1

.greater:
mov eax, 1
jmp .l2
.less:
mov eax, -1
jmp .l2
.equal:
xor eax, eax

.l2:
pop esi
pop edi
pop ebp
ret
.end:

; + 8 ptr
; +12 val
; +16 num
memchr:
push ebp
mov ebp, esp
push ebx

mov eax, dword [ebp +  8]	; ptr
mov ebx, dword [ebp + 12]	; val
mov dl, byte [ebp + 16]	; num
xor ecx, ecx

.l1:
cmp edx, ecx
jl .zero		; if ecx >= edx then return NULL;
mov bh, byte [eax + ecx]
cmp bh, bl
jz .found

inc ecx
jmp .l1

.found:
add eax, ecx
jmp .return
.zero:
xor eax, eax

.return:
pop ebx
pop ebp
ret
.end:

; + 8 str
; +12 ch
strchr:
push ebp
mov ebp, esp

mov eax, dword [ebp +  8]
mov dl, byte [ebp + 12]
xor ecx, ecx
.l1:
mov dh, byte [eax + ecx]
cmp dh, dl
jz .found
cmp dh, byte 0
jz .zero

inc ecx
jmp .l1


.found:
add eax, ecx
pop ebp
ret
.end:

.zero:
xor eax, eax
pop ebp
ret

; + 8 str
; +12 num
strnlen:
push ebp
mov ebp, esp

xor eax, eax
mov edx, dword [ebp +  8]
mov ecx, dword [ebp + 12]
.l1:
cmp eax, ecx
jge .l2
cmp byte [edx + eax], byte 0
jz .l2

inc eax
jmp .l1

.l2:

pop ebp
ret
.end:

; +4 str
strlwr:

mov eax, dword [esp + 4]
xor ecx, ecx

.l1:
mov dl, byte [eax + ecx]
cmp dl, byte 0
je .ret
cmp dl, byte 'A'
jl .l2
cmp dl, byte 'Z'
jg .l2
add dl, byte 0x20
mov byte [eax + ecx], dl
.l2:
inc ecx
jmp .l1

.ret:
ret
.end:

; +4 str
strupr:

mov eax, dword [esp + 4]
xor ecx, ecx

.l1:
mov dl, byte [eax + ecx]
cmp dl, byte 0
je .ret
cmp dl, byte 'a'
jl .l2
cmp dl, byte 'z'
jg .l2
sub dl, byte 0x20
mov byte [eax + ecx], dl
.l2:
inc ecx
jmp .l1

.ret:
ret
.end:

; + 8 str
; +12 num
strnlwr:
push ebp
mov ebp, esp
push ebx

mov eax, dword [ebp + 8]
mov ebx, dword [ebp +12]
xor ecx, ecx

.l1:
cmp ecx, ebx
jge .l2
mov dl, byte [eax + ecx]
cmp dl, byte 'A'
jl .l3
cmp dl, byte 'Z'
jg .l3

add dl, byte 0x20
mov byte [eax + ecx], dl

.l3:
inc ecx
jmp .l1

.l2:

pop ebx
pop ebp
ret
.end:

; + 8 str
; +12 num
strnupr:
push ebp
mov ebp, esp
push ebx

mov eax, dword [ebp + 8]
mov ebx, dword [ebp +12]
xor ecx, ecx

.l1:
cmp ecx, ebx
jge .l2
mov dl, byte [eax + ecx]
cmp dl, byte 'a'
jl .l3
cmp dl, byte 'z'
jg .l3

sub dl, byte 0x20
mov byte [eax + ecx], dl

.l3:
inc ecx
jmp .l1

.l2:

pop ebx
pop ebp
ret
.end:

; +4 str
strrvs:

push dword [esp + 4]
call strlen
add esp, byte 4

push eax
push dword [esp + 8]
call strnrvs
add esp, byte 8

ret
.end:

; + 8 str
; +12 num
strnrvs:
push ebp
mov ebp, esp
push ebx

mov eax, dword [ebp + 8]	; str
xor ecx, ecx				; start
mov ebx, dword [ebp + 12]	; end
dec ebx

.l1:
cmp ecx, ebx
jge .l2
mov dl, byte [eax + ecx]
mov dh, byte [eax + ebx]
mov byte [eax + ecx], dh
mov byte [eax + ebx], dl

inc ecx
dec ebx
jmp .l1

.l2:
pop ebx
pop ebp
ret
.end:
