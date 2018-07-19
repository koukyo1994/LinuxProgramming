section .data
    example1: db 2, 72, 69, 76, 76, 79, 10, 3
    example2: times 999 db 72, 10
    example3: dw 999, 10

section .text
global _start
_start:
    mov rax, 1
    mov rdi, 1
    mov rsi, example1
    mov rdx, 8
    syscall

    mov rsi, example2
    mov rdx, 1002
    syscall

    mov rsi, example3
    mov rdx, 3
    syscall

    mov rax, 60
    mov rdi, rdi
    syscall
