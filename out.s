.intel_syntax noprefix
.globl main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 800
    mov rax, 10
    push rax
    mov rax, 5
    pop rbx
    add rax, rbx
    push rax
    mov rax, 2
    pop rbx
    imul rax, rbx
    mov rsp, rbp
    pop rbp
    ret
