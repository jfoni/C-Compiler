.intel_syntax noprefix
.globl main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 800
    mov rax, 0
    mov [rbp - 8], rax
    mov rax, [rbp - 8]
    cmp rax, 0
    je .Lelse_0
    mov rax, 100
    mov rsp, rbp
    pop rbp
    ret
    jmp .Lend_0
.Lelse_0:
    mov rax, 200
    mov rsp, rbp
    pop rbp
    ret
.Lend_0:
