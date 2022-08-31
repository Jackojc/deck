; UTILITIES

%macro ___count 0 ; ( -> x )
; Pushes the stack element count to the stack.
	mov rbx, rbp
	sub rbx, rsp
	shr rbx, 3   ; Divide by `8`.
	push rax
	mov rax, rbx
%endmacro

%macro ___clear 0 ; ( ... -> )
; Empty the stack.
	mov rsp, rbp
	mov rax, 0
%endmacro



