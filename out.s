.intel_syntax noprefix
.globl main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 800
    mov rax, 5
    mov [rbp - 8], rax
.Lloop_start_0:
    mov rax, [rbp - 8]
    cmp rax, 0
    je .Lloop_end_0
    mov rax, 1
    push rax
    mov rax, [rbp - 8]
    pop rbx
    sub rax, rbx
    mov [rbp - 8], rax
    jmp .Lloop_start_0
.Lloop_end_0:
    mov rax, [rbp - 8]
    mov rsp, rbp
    pop rbp
    ret
