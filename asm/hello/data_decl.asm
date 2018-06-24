section .data
    example1: db 5, 16, 8, 4, 2, 1, 10
    example2: times 999 db 42, 10
    example3: dw 999, 10

section .text
global _start
_start:
    mov rax, 1
    mov rdi, 1
    mov rsi, example1
    mov rdx, 7
    syscall

    mov rsi, example2
    mov rdx, 1000
    syscall

    mov rsi, example3
    mov rdx, 3
    syscall

    mov rax, 60
    mov rdi, rdi
    syscall
