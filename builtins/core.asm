; BOILERPLATE
%macro ___header 0
	bits 64
	global _start
	section .text

	_start:
	lea rsp, [rsp - 100 * 8] ; Allocate 100 elements for the deque.

	mov r9, rsp  ; Default marker.
	mov rbp, rsp ; Pointer to opposite of deque.
%endmacro


; CONSTANTS
%macro ___true 0 ; ( -> t )
	___push 1
%endmacro

%macro ___false 0 ; ( -> f )
	___push 0
%endmacro


; CONTROL FLOW
%macro ___mark 0 ; ( -> | mark )
; Store the current base pointer value to the stack
; and then set the base pointer to the current stack
; pointer.
	___push r9
	___deque_push
	mov r9, rsp
%endmacro

%macro ___unmark 0 ; ( mark -> )
; Set the marker to the value on top of the stack.
	___deque_pop
	pop r9
%endmacro

%macro ___here 0 ; ( -> ip )
; Pushes the current instruction pointer to the stack.
	%%here:
	___push %%here
%endmacro

%macro ___go 0 ; ( addr -> )
; Jumps to the address on the top of the stack.
	pop rax
	jmp rax
%endmacro

%macro ___call 1 ; ( -> )
; Pushes the address following this instruction to the stack
; before jumping to the given argument.
	___push %%after
	jmp %1
	%%after:
%endmacro

%macro ___call_go 0 ; ( addr -> )
; Calls a function whose address is on top of the stack.
; This is essentially a runtime version of `___call` which
; takes a value on top of the stack to jump to and also pushes
; a continuation address for the procedure to jump to afterwards.
	pop rax
	___push %%after
	jmp rax
	%%after:
%endmacro

%macro ___choose 0 ; ( cond t f -> q )
; Checks the condition and collapses to either
; `t` or `f` based on its value.
	pop rax ; f
	pop rbx ; t
	pop rcx ; cond

	cmp rcx, 1
	cmove rax, rbx  ; Move the the `true` case to `rax` if `rcx` is `1`.
	___push rax
%endmacro

%macro ___branch 0 ; ( cond t f -> )
; Same as `___choose` except we _jump_ to either of the true/false values
; depending on the value of `cond`.
	pop rax ; f
	pop rbx ; t
	pop rcx ; cond

	cmp rcx, 1
	cmove rax, rbx  ; Move the the `true` case to `rax` if `rcx` is `1`.
	jmp rax
%endmacro

%macro ___if 0 ; ( cond addr -> )
; Jump to address if top of stack is truthy.
; These instructions may seem suboptimal but it's due
; to a quirk in the x86 family of conditional `mov`
; instructions not allowing use of far jumps. To
; get around this, a conditional jump is used to
; guard a far jump.
	pop rax ; x
	pop rbx ; cond
	test rbx, rbx
	jz %%skip
	jmp rax
	%%skip:
%endmacro

