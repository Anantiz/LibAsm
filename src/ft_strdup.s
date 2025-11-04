;char *strdup(const char *s);

; Registers:
; Args: rdi, rsi, rdx, rcx, r8, r9 ; Extra unused args-registers can be used
; FreeUse: rax, r10, r11
; MustBeRestored: rbx, rbp, r12, r13, r14, r15

global ft_strdup
extern malloc

bits 64

section .text

ft_strlen:
	xor rax, rax
.loop_enter:
    cmp byte [rdi], 0
	je .loop_exit
    inc rax
    inc rdi
    jmp .loop_enter
.loop_exit:
    ret


ft_strdup:
    ; get len
    push rdi
    call ft_strlen
    inc rax ; add size for null terminator


    ; call malloc and null check
    mov rdi, rax
    push rax ; save len
    sub rsp, 8
    call malloc wrt ..plt
    add rsp, 8
    test rax, rax
    je .exit_error

    ; rax           ; new_ptr
    pop rsi         ; len        use an available register
    pop rdi         ; old_str    use an available register
    mov rcx, rax    ; write_ptr  use an available register
    add rsi, rax    ; write_end_ptr

; We will comapre the write ptr to the end ptr instead of checking for lenght (less arithmetic)
.copy_loop:
    cmp rcx, rsi
    je .exit

    ; Already used: rax, rcx -> Use register D for char
    mov dl, [rdi] ; read char
    mov [rcx], dl ; write char

    inc rdi
    inc rcx
    jmp .copy_loop
.exit:
    ret
.exit_error
    ; unroll the stack: move the stack ptr back to it's original place (pop the two vals)
    add rsp, 16

    xor rax, rax
    ret
