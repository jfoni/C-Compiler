.intel_syntax noprefix
.globl main
main:
    mov rax, 5
    push rax
    mov rax, 10
    push rax
    mov rax, 20
    pop rbx
    add rax, rbx
    pop rbx
    sub rax, rbx
    ret
