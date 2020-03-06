section .text
global sqrt:function (sqrt.end - sqrt)
global sqrtf:function (sqrtf.end - sqrtf)
global sin:function (sin.end - sin)
global sinf:function (sinf.end - sinf)
global cos:function (cos.end - cos)
global cosf:function (cosf.end - cosf)
global fabs:function (fabs.end - fabs)
global fabsf:function (fabsf.end - fabsf)
global tan:function (tan.end - tan)
global tanf:function (tanf.end - tanf)
global rad2deg:function (rad2deg.end - rad2deg)
global deg2rad:function (deg2rad.end - deg2rad)
global round:function (round.end - round)
global roundf:function (roundf.end - roundf)
global fmin:function (fmin.end - fmin)
global fminf:function (fminf.end - fminf)
global fmax:function (fmax.end - fmax)
global fmaxf:function (fmaxf.end - fmaxf)

; +4 x
sqrt:
fld qword [esp + 4]
fsqrt
ret
.end:

; +4 x
sqrtf:
fld dword [esp + 4]
fsqrt
ret
.end:

; +4 x
sin:
fld qword [esp + 4]
fsin
ret
.end:

; +4 x
sinf:
fld dword [esp + 4]
fsin
ret
.end:

; +4 x
cos:
fld qword [esp + 4]
fcos
ret
.end:

; +4 x
cosf:
fld dword [esp + 4]
fcos
ret
.end:

fabs:
fld qword [esp + 4]
fabs
ret
.end:

fabsf:
fld dword [esp + 4]
fabs
ret
.end:

; +4 x
tan:
fld qword [esp + 4]
fcos
fld qword [esp + 4]
fsin
fdiv st0, st1
ret
.end:

tanf:
fld dword [esp + 4]
fsincos
fdiv
ret
.end:

_180_pi: dd 57.2958
; +4 x
rad2deg:
fld qword [esp + 4]
fmul dword [_180_pi]
ret
.end:

pi_180: dd 0.0174533
; +4 x
deg2rad:
fld qword [esp + 4]
fmul dword [pi_180]
ret
.end:

; +8 x
round:
push ebp
mov ebp, esp
sub esp, byte 8

fld qword [ebp + 8]
fistp qword [esp]
fild qword [esp]

add esp, byte 8
pop ebp
ret
.end:

; +8 x
roundf:
push ebp
mov ebp, esp
sub esp, byte 4

fld dword [ebp + 8]
fistp dword [esp]
fild dword [esp]

add esp, byte 4
pop ebp
ret
.end:

; + 4 a
; +12 b
fmin:
; y = a < b ? a : b;
fld qword [esp +  4]
fld qword [esp + 12]
fcompp
jl .l1
fld qword [esp + 12]
ret
.l1:
fld qword [esp +  4]
ret
.end:

; +4 a
; +8 b
fminf:
fld dword [esp + 4]
fld dword [esp + 8]
fcompp
jl .l1
fld dword [esp + 8]
ret
.l1:
fld dword [esp + 4]
ret
.end:

; + 4 a
; +12 b
fmax:
fld qword [esp + 4]
fld qword [esp + 8]
fcompp
jg .l1
fld qword [esp + 8]
ret
.l1:
fld qword [esp + 4]
ret
.end:

; +4 a
; +8 b
fmaxf:
fld dword [esp + 4]
fld dword [esp + 8]
fcompp
jg .l1
fld dword [esp + 8]
ret
.l1:
fld dword [esp + 4]
ret
.end:
