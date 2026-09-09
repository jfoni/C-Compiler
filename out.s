.intel_syntax noprefix
.globl main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 800
    mov rax, 10
    mov [rbp - 8], rax
    mov rax, 20
    mov [rbp - 16], rax
    mov rax, [rbp - 16]
    push rax
    mov rax, [rbp - 8]
    pop rbx
    add rax, rbx
    mov rsp, rbp
    pop rbp
    ret
