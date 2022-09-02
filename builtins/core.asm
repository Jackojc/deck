; BOILERPLATE
%macro ___res_header 0
	bits 64
	global _start
	section .text
	_start:
	lea rsp, [rsp - 100 * 8] ; Allocate 100 elements for the deque.
	mov rbp, rsp
%endmacro

%macro ___res_footer 0
	mov rax, 60
	xor rdi, rdi
	syscall
%endmacro

%macro ___res_func 0
	; pop rax
%endmacro


; CONSTANTS
%macro ___true 0 ; ( -> x )
	push rax
	mov rax, 1
%endmacro

%macro ___false 0 ; ( -> x )
	push rax
	xor rax, rax
%endmacro


; CONTROL FLOW
; %macro ___mark 0 ; ( -> x )
; ; Store the current base pointer value to the stack
; ; and then set the base pointer to the current stack
; ; pointer.
; 	push rbp
; 	mov rax, rbp
; 	mov rbp, rsp
; %endmacro

; %macro ___unmark 0 ; ( x -> y )
; ; Set the base pointer to the value at the top of the
; ; stack after storing the current base pointer to the
; ; stack.
; ; This is effectively just a swap of the top element
; ; and the base pointer.
; 	xchg rax, rbp
; %endmacro

%macro ___here 0 ; ( -> x )
; Pushes the current instruction pointer to the stack.
	%%here:
	push rax
	mov rax, %%here
%endmacro

%macro ___go 0 ; ( x -> )
; Jumps to the address on the top of the stack.
	mov rbx, rax
	pop rax
	jmp rbx
%endmacro

%macro ___call 1 ; ( -> )
; Shorthand for pushing a return address to the following
; instruction.
; This pushes the current top of the stack and then jumps to
; the named function while also pushing the return address
; to the top of the stack. We need to pop this address to
; `rax` using the function header boilerplate.
	push rax
	mov rax, %%after
	jmp %1
	%%after:
%endmacro

%macro ___choose 0 ; ( cond t f -> q )
; Checks the condition and collapses to either
; `t` or `f` based on its value.
; We test if `rax` is zero first and then replace
; `rax` with the top of stack and move the next element
; into `rbx`. The zero flag is still set at this point
; so we use `cmov` to conditionally move `rbx` into `rax`.
	pop rbx
	pop rcx
	cmp rcx, 1
	cmove rax, rbx
%endmacro

%macro ___if 0 ; ( cond x -> )
; Jump to address if top of stack is truthy.
; These instructions may seem suboptimal but it's due
; to a quirk in the x86 family of conditional `mov`
; instructions not allowing use of far jumps. To
; get around this, a conditional jump is used to
; guard a far jump.
	mov rcx, rax
	pop rbx
	pop rax
	test rbx, rbx
	jz %%skip
	jmp rcx
	%%skip:
%endmacro

