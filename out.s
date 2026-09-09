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
    mov rcx, rax
    mov rax, rbx
    cmp rax, rcx
    setg al
    movzx rax, al
    cmp rax, 0
    je .Lelse_0
    mov rax, [rbp - 8]
    push rax
    mov rax, 2
    pop rbx
    imul rax, rbx
    mov rsp, rbp
    pop rbp
    ret
.Lelse_0:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret
