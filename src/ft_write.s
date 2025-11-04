; ssize_t write(int fd, const void *buf, size_t count)

; Registers:
; Args: rdi, rsi, rdx, rcx, r8, r9 ; Extra unused args-registers can be used
; FreeUse: rax, r10, r11
; MustBeRestored: rbx, rbp, r12, r13, r14, r15

global ft_write

bits 64

extern __errno_location ; returns a ptr to a thread specific errno var

section .text

ft_write:
    ;rdi has FD already
    ;rsi has ptr already
    ;rdx has size already
    mov rax, 1 ; 1=write syscall
    syscall
    test rax, rax ; if negative we have to set errno
    js .set_error
    ret

.set_error
    ; Syscalls return the negative Error code
    ; (negative such that SF flag is set)
    ; make it positive for errno
    neg rax

    ; Store the rax somewhere safe before the errno call
    push rax ; Put in on the stack

    call __errno_location wrt ..plt; size_t __errno_location(void)
    ;wrt ..plt is a tool for the linker to find the function pointer by name from a specific table (better for dynamic library)
    pop rdi; get the error back
    mov [rax], edi ; set the error code in the errno, use 'edi' to store only 32-bits
    mov rax, -1 ; return error
    ret
