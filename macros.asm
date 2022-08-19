; MACROS

; `rax` is the top element of the stack and these
; macros handle the boilerplate of shifting things into
; either `rax` or the actual stack.

; `rax` = top of stack
; `r8`  = continuation register


; STACK PRIMITIVES
%macro m_push 1 ; ( x -> y )
; Move a literal into `rax` after pushing
; the old top of stack.
	push rax
	mov rax, %1
%endmacro

%macro m_pop 0 ; ( x -> )
; Pop the top element of the stack.
; Popping to `rax` implicitly overwrites the top element.
	pop rax
%endmacro

%macro m_drop 0 ; ( x -> )
; Alias for `m_pop`.
	m_pop
%endmacro

%macro m_swap 0 ; ( a b -> b a )
; Swap the top two elements of the stack.
	xchg rax, [rsp]
%endmacro

%macro m_dup 0 ; ( x -> x x )
; Duplicate the top element of the stack.
; This is fairly trivial to do by just pushing
; `rax` since the value currently in `rax` will
; be preserved after the push.
	push rax
%endmacro

%macro m_over 0 ; ( a b -> a b a )
; Copy the element over the top element to the top
; of the stack.
	push rax
	mov rax, [rsp - 8]
%endmacro

%macro m_nip 0 ; ( a b -> b )
; Remove the element underneath the top element.
	sub rsp, 8
%endmacro


; CONSTANTS
%macro m_true 0 ; ( -> x )
	m_push 1
%endmacro

%macro m_false 0 ; ( -> x )
	m_push 0
%endmacro


; ARITHMETIC
; Most of these operations work in the same way by
; first popping the top two elements, performing
; the operation and then pushing the result back to
; the stack.
%macro m_add 0 ; ( a b -> q )
	pop rbx
	add rax, rbx
%endmacro

%macro m_sub 0 ; ( a b -> q )
	pop rbx
	sub rax, rbx
%endmacro

%macro m_mul 0 ; ( a b -> q )
	pop rbx
	imul rax, rbx
%endmacro

%macro m_div 0 ; ( a b -> q )
; We need to sign extend `rax` to `rdx` because
; `idiv` operates on 128 bits.
	pop rbx
	cqo
	idiv rbx
%endmacro

%macro m_mod 0 ; ( a b -> q )
; Remainder is mostly the same as division on x86 but
; here we move the remainder (stored in `rdx`) into `rax`
; which is the top of the stack.
	pop rbx
	cqo
	idiv rbx
	mov rax, rdx
%endmacro

%macro m_lsh 0 ; ( a b -> q )
	pop rcx
	sal rax, cl
%endmacro

%macro m_rsh 0 ; ( a b -> q )
	pop rcx
	sar rax, cl
%endmacro

%macro m_inc 0 ; ( x -> y )
; Increment top element of stack by one.
	inc rax
%endmacro

%macro m_dec 0 ; ( x -> y )
; Decrement top element of stack by one.
	dec rax
%endmacro

%macro m_neg 0 ; ( x -> y )
; Make the top element negative (two's complement).
	neg rax
%endmacro

%macro m_abs 0 ; ( x -> y )
; Return the absolute value of the top stack element.
	pop rbx
	xor rax, rbx
	sub rax, rbx
%endmacro


; LOGICAL
; These operations work on booleans.
%macro m_and 0 ; ( a b -> q )
	pop rbx
	test rax, rax
	setne al
	test rbx, rbx
	setne bl
	and al, bl
	movzx rax, al
%endmacro

%macro m_or 0 ; ( a b -> q )
	pop rbx
	or rax, rbx
	setne al
	movzx rax, al
%endmacro

%macro m_xor 0 ; ( a b -> q )
	pop rbx
	xor rax, rbx
	setne al
	movzx rax, al
%endmacro

%macro m_not 0 ; ( a b -> q )
	test rax, rax
	setne al
	movzx rax, al
%endmacro


; BITWISE
; These operations work on individual bits.
%macro m_band 0 ; ( a b -> q )
	pop rbx
	and rax, rbx
%endmacro

%macro m_bor 0 ; ( a b -> q )
	pop rbx
	or rax, rbx
%endmacro

%macro m_bxor 0 ; ( a b -> q )
	pop rbx
	xor rax, rbx
%endmacro

%macro m_bnot 0 ; ( a b -> q )
	not rbx
%endmacro


; COMPARISONS
%macro m_eq 0 ; ( a b -> q )
; Checks the top two elements of the stack
; for equality and then collapses to either
; `1` or `0`.
	pop rbx
	test rax, rbx
	sete al
	movzx rax, al
%endmacro

%macro m_neq 0 ; ( a b -> q )
; Checks the top two elements of the stack
; for inequality and then collapses to either
; `1` or `0`.
	pop rbx
	test rax, rbx
	setne al
	movzx rax, al
%endmacro

%macro m_lt 0 ; ( a b -> q )
	pop rbx
	test rax, rbx
	setl al
	movzx rax, al
%endmacro

%macro m_lte 0 ; ( a b -> q )
	pop rbx
	test rax, rbx
	setle al
	movzx rax, al
%endmacro

%macro m_gt 0 ; ( a b -> q )
	pop rbx
	test rax, rbx
	setg al
	movzx rax, al
%endmacro

%macro m_gte 0 ; ( a b -> q )
	pop rbx
	test rax, rbx
	setge al
	movzx rax, al
%endmacro


; UTILITIES
%macro m_choose 0 ; ( t f cond -> q )
; Checks the condition and collapses to either
; `t` or `f` based on its value.
; We test if `rax` is zero first and then replace
; `rax` with the top of stack and move the next element
; into `rbx`. The zero flag is still set at this point
; so we use `cmov` to conditionally move `rbx` into `rax`.
	test rax, rax

	mov rax, [rsp]     ; false value
	mov rbx, [rsp - 8] ; true value

	cmovne rax, rbx    ; move `rbx` to `rax` if `rax` != 0
	sub rsp, 16
%endmacro

%macro m_here 0 ; ( -> x )
; Pushes the current instruction pointer to the stack.
	m_push rip
%endmacro

%macro m_go 0 ; ( x -> )
; Jumps to the address on the top of the stack.
	mov rax, rbx
	m_pop
	jmp rbx
%endmacro

%macro m_read 0 ; ( -> x )
; Reads a single character from stdin and pushes
; it to the stack.
; We grow the stack by a quad word and then write the
; character to this newly created space. We then swap
; the top two stack elements so the character is in `rax`
; and the previous element is at `[rsp]`.
	add rsp, 8

	mov rax, 0   ; read
	mov rdi, 0   ; stdin
	mov rsi, rsp ; dest
	mov rdx, 1   ; count
	syscall

	m_swap
%endmacro

%macro m_write 0 ; ( x -> )
; Write the character on the top of the stack to stdout.
; First we take the top element of the stack from `rax`
; and push it to actual stack so that we can address it
; in the write syscall. Afterwards, we restore the stack
; by removing the character we wrote and moving the next
; element up to the top in `rax`.
	push rax

	mov rax, 1   ; write
	mov rdi, 1   ; stdout
	mov rsi, rsp ; source
	mov rdx, 1   ; count
	syscall

	sub rsp, 8
	pop rax
%endmacro

%macro m_min 0 ; ( a b -> q )
; Collapses to the minimum of the top two stack
; values.
	pop rbx
	cmp rax, rbx
	cmovl rax, rbx
%endmacro

%macro m_max 0 ; ( a b -> q )
; Collapses to the maximum of the top two stack
; values.
	pop rbx
	cmp rax, rbx
	cmovg rax, rbx
%endmacro
