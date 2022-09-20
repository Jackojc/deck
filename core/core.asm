; CORE/BOILERPLATE
%macro ___header 0
	bits 64
	global _start
	section .text

	_start:
	lea rsp, [rsp - 100 * 8] ; Allocate 100 elements for the deque.

	mov r9, rsp  ; Default marker.
	mov rbp, rsp ; Pointer to opposite of deque.
%endmacro

%macro ___push 1 ; ( -> x )
; Push literal to the stack.
	push qword %1
%endmacro

%macro ___label 1
	%1:
%endmacro

%macro ___call 1 ; ( -> )
; Pushes the address following this instruction to the stack
; before jumping to the given argument.
	call %1
%endmacro

%macro ___go 0 ; ( addr -> )
; Jumps to the address on the top of the stack.
	pop rax
	jmp rax
%endmacro

%macro ___here 0 ; ( -> ip )
; Pushes the current instruction pointer to the stack.
	%%here:
	push %%here
%endmacro

%macro ___count 0 ; ( -> n )
; Pushes the stack element count to the stack.
	mov rax, r9  ; `r9` is the current marker.
	sub rax, rsp
	shr rax, 3   ; Divide by `8` because we're using quad words on x86-64.
	push rax
%endmacro

%macro ___word 0 ; ( -> x )
; Pushes the word size in bits to the stack.
; Ideally this should enable Deck to be more
; platform agnostic.
	push qword 64
%endmacro

%macro ___byte 0 ; ( -> x )
; Pushes the byte size in bits to the stack.
	push qword 8
%endmacro

