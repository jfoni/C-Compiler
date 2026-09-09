.intel_syntax noprefix
.globl main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 800
    mov rax, 10
    mov [rbp - 8], rax
.Lloop_start_0:
    mov rax, [rbp - 8]
    push rax
    mov rax, 0
    pop rbx
    mov rcx, rax
    mov rax, rbx
    cmp rax, rcx
    setg al
    movzx rax, al
    cmp rax, 0
    je .Lloop_end_0
    mov rax, [rbp - 8]
    push rax
    mov rax, 2
    pop rbx
    mov rcx, rax
    mov rax, rbx
    cqo
    idiv rcx
    mov rax, rdx
    push rax
    mov rax, 0
    pop rbx
    mov rcx, rax
    mov rax, rbx
    cmp rax, rcx
    sete al
    movzx rax, al
    cmp rax, 0
    je .Lelse_1
    mov rax, [rbp - 8]
    push rax
    mov rax, 2
    pop rbx
    mov rcx, rax
    mov rax, rbx
    sub rax, rcx
    mov [rbp - 8], rax
    jmp .Lend_1
.Lelse_1:
    mov rax, [rbp - 8]
    push rax
    mov rax, 1
    pop rbx
    mov rcx, rax
    mov rax, rbx
    sub rax, rcx
    mov [rbp - 8], rax
.Lend_1:
    jmp .Lloop_start_0
.Lloop_end_0:
    mov rax, [rbp - 8]
    mov rsp, rbp
    pop rbp
    ret
