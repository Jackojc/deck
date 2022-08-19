%include "macros.asm"

bits 64
global _start

; CODE
section .text
_start:
	mov rax, 60
	xor rdi, rdi
	syscall

