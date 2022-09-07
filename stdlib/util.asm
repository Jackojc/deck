; UTILITIES

sys_exit: ; ( x -> )
; Exit syscall.
; We overwrite the top of stack here but we're exiting so who cares.
	mov rax, 60
	pop rdi
	syscall

sys_abort: ; ( -> )
; Exit with a `1` status to signify that something went wrong.
	mov rax, 60
	mov rdi, 1
	syscall

sys_ok: ; ( -> )
; Exit with `0` status to signify that the program
; performed correctly.
	mov rax, 60
	xor rdi, rdi
	syscall
