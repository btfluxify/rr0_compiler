    .intel_syntax noprefix
    .global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov eax, 0
    mov DWORD PTR [rbp - 1], eax
    mov eax, 123
    mov DWORD PTR [rbp - 5], eax
    mov eax, DWORD PTR [rbp - 1]
    leave
    ret
