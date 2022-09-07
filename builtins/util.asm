; UTILITIES

%macro ___count 0 ; ( -> x )
; Pushes the stack element count to the stack.
	mov rax, r9  ; `r9` is the current marker.
	sub rax, rsp
	shr rax, 3   ; Divide by `8` because we're using quad words on x86-64.
	push rax
%endmacro

%macro ___clear 0 ; ( ... -> )
; Empty the stack.
	mov rsp, r9  ; Set the stack pointer to the current marker.
%endmacro

