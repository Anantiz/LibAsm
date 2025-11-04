; ssize_t read(int fd, void *buf, size_t count);

; Registers:
; Args: rdi, rsi, rdx, rcx, r8, r9 ; Extra unused args-registers can be used
; FreeUse: rax, r10, r11
; MustBeRestored: rbx, rbp, r12, r13, r14, r15

global ft_read

bits 64

extern __errno_location ; returns a ptr to a thread specific errno var

section .text

ft_read:
    mov rax, 0 ; 0=read syscall
    syscall
    test rax, rax ; if negative we have to set errno
    js .set_error
    ret

.set_error
    neg rax
    push rax
    call __errno_location wrt ..plt
    pop rdi
    mov [rax], edi
    mov rax, -1
    ret
