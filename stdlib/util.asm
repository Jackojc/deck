; UTILITIES

sys_exit: ; ( x -> )
; Exit syscall.
; We overwrite the top of stack here but we're exiting so who cares.
	mov rdi, rax
	mov rax, 60
	syscall

sys_abort: ; ( -> )
; Exit syscall.
; We overwrite the top of stack here but we're exiting so who cares.
	mov rax, 60
	mov rdi, 1
	syscall
