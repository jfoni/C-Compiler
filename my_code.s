.intel_syntax noprefix
.globl main
main:
    mov rax, 5
    push rax
    mov rax, 10
    pop rbx
    add rax, rbx
    ret
