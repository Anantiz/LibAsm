; cmp A, B will set: py `ZF = 1 if (B-A) == 0 else 0`
bits 64
section .text
global ft_strlen
; const char *rdi
ft_strlen:
	xor rax, rax ; rax = 0
.loop_enter:
    cmp byte [rdi], 0 ; "byte" is required cuz '0' is a constant and that stupid ass syntax/compiler can't just compare a whole byte by himself like a grown man
	je .loop_exit; If ZF==1
    inc rax; move counter
    inc rdi; move ptr
    jmp .loop_enter
.loop_exit:
    ret
