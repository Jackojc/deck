bits 64
global _start

; DATA
section .bss
io_buf: resb 1 ; Temporary buffer for r/w syscalls.

; CODE
section .text
_start: jmp main

io_read: ; ( ret -> chr )
; Read a single character from stdin and push it
; to the stack as a qword.

	pop r8 ; ret

	mov rax, 0      ; read
	mov rdi, 0      ; stdin
	mov rsi, io_buf ; dest
	mov rdx, 1      ; count
	syscall

	; If rax is 0, we've reached EOF. If rax is _not_
	; zero, it means that we have successfully read a
	; character into the buffer and we conditionally
	; load it into rax. In effect, the value of rax
	; after this is the character that we read.
	test rax, rax
	cmovne rax, [io_buf]

	push rax
	jmp r8

io_write: ; ( chr ret -> )
; Pop a character from the stack
; and write it to stdout.

	pop r8  ; ret
	pop rax ; chr

	mov [io_buf], al

	mov rax, 1      ; write
	mov rdi, 1      ; stdout
	mov rsi, io_buf ; source
	mov rdx, 1      ; count
	syscall

	jmp r8

sys_exit: ; ( sts -> )
; Exits with status `0`.
	mov rax, 60  ; exit
	pop rdi      ; status
	syscall

main: ; ( -> )
	main_read:
	push main_write
	jmp io_read

	main_write:
	cmp qword [rsp], 0
	je main_end

	push main_read
	jmp io_write

	main_end:
	push 0
	jmp sys_exit

; skip whitespace
; skip comment

; compile
; compile_def
; compile_expr

; compile_header
; compile_footer

; compile_hex
; compile_op
; compile_name
; compile_data

