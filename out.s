.intel_syntax noprefix
.globl main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 800
    mov rax, 10
    mov [rbp - 8], rax
    mov rax, [rbp - 8]
    push rax
    mov rax, 5
    pop rbx
    add rax, rbx
    mov [rbp - 8], rax
    mov rax, [rbp - 8]
    mov rsp, rbp
    pop rbp
    ret
