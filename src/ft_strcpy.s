; Registers:
; Args: rdi, rsi, rdx, rcx, r8, r9 ; Extra unused args-registers can be used
; FreeUse: rax, r10, r11
; MustBeRestored: rbx, rbp, r12, r13, r14, r15

; char* strcpy(char *dst, const char *src);

global ft_strcpy

bits 64

section .text

ft_strcpy:
    mov rax, rdi; strcpy return dest
.L1:

    ; mov [rdi], [rsi] ; ILLEGAL instruction,
    ; fck ASM, wtf; I'll just write in binary at this point
    ; use cl to only take the lowest byte (char)
    mov cl, [rsi]
    mov [rdi], cl

    test cl, cl
    je .L2

    inc rdi
    inc rsi
    jmp .L1
.L2
    ret


; We move First (in case it's a null string: We still need to copy null terminator)
; Then we compare the value
; then we increment and loop
