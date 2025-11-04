; Registers:
; Args: rdi, rsi, rdx, rcx, r8, r9 ; Extra unused args-registers can be used
; FreeUse: rax, r10, r11
; MustBeRestored: rbx, rbp, r12, r13, r14, r15

;int strcmp(const char *s1, const char *s2);

global ft_strcmp

bits 64
section .text
ft_strcmp:
    xor rax, rax ; Init to 0 for correct return
.L1:
    ; load chars
    mov al, [rdi] ; s1
    mov cl, [rsi] ; s2

    ; cmp and control flow
    cmp al, cl ; *s2 - *s1
    jne .L2;

    ; null-check
    test cl, cl
    je .L2

    ; next
    inc rsi
    inc rdi
    jmp .L1
.L2:
    ; ~~No need to set the A register because we init it to 0 and later it only contains the subs~~
    ; Actually you still do because the sign is at the wrong place
    sub rax, rcx
    ret
