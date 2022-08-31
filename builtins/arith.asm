; ARITHMETIC
; Most of these operations work in the same way by
; first popping the top two elements, performing
; the operation and then pushing the result back to
; the stack.


; MATHS
%macro ___add 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	add rax, rbx
%endmacro

%macro ___sub 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	sub rax, rbx
%endmacro

%macro ___mul 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	imul rax, rbx
%endmacro

%macro ___div 0 ; ( a b -> q )
; We need to sign extend `rax` to `rdx` because
; `idiv` operates on 128 bits.
	pop rbx
	xchg rax, rbx
	cqo
	idiv rbx
%endmacro

%macro ___mod 0 ; ( a b -> q )
; Remainder is mostly the same as division on x86 but
; here we move the remainder (stored in `rdx`) into `rax`
; which is the top of the stack.
	pop rbx
	xchg rax, rbx
	cqo
	idiv rbx
	mov rax, rdx
%endmacro

%macro ___lsh 0 ; ( a b -> q )
	pop rcx
	xchg rax, rcx
	sal rax, cl
%endmacro

%macro ___rsh 0 ; ( a b -> q )
	pop rcx
	xchg rax, rcx
	sar rax, cl
%endmacro

%macro ___inc 0 ; ( x -> y )
; Increment top element of stack by one.
	inc rax
%endmacro

%macro ___dec 0 ; ( x -> y )
; Decrement top element of stack by one.
	dec rax
%endmacro

%macro ___neg 0 ; ( x -> y )
; Make the top element negative (two's complement).
	neg rax
%endmacro

%macro ___abs 0 ; ( x -> y )
; Return the absolute value of the top stack element.
	mov rbx, rax
	neg rax
	cmovl rax, rbx
%endmacro

%macro ___min 0 ; ( a b -> q )
; Collapses to the minimum of the top two stack
; values.
	pop rbx
	cmp rax, rbx
	cmovg rax, rbx
%endmacro

%macro ___max 0 ; ( a b -> q )
; Collapses to the maximum of the top two stack
; values.
	pop rbx
	cmp rax, rbx
	cmovl rax, rbx
%endmacro


; LOGICAL
; These operations work on booleans.
%macro ___and 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	test rax, rax
	setne al
	test rbx, rbx
	setne bl
	and al, bl
	movzx rax, al
%endmacro

%macro ___or 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	or rax, rbx
	setne al
	movzx rax, al
%endmacro

%macro ___xor 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	xor rax, rbx
	setne al
	movzx rax, al
%endmacro

%macro ___not 0 ; ( a b -> q )
	test rax, rax
	setne al
	movzx rax, al
%endmacro


; BITWISE
; These operations work on individual bits.
%macro ___band 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	and rax, rbx
%endmacro

%macro ___bor 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	or rax, rbx
%endmacro

%macro ___bxor 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	xor rax, rbx
%endmacro

%macro ___bnot 0 ; ( a b -> q )
	not rbx
%endmacro


; COMPARISONS
%macro ___eq 0 ; ( a b -> q )
; Checks the top two elements of the stack
; for equality and then collapses to either
; `1` or `0`.
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	sete al
	movzx rax, al
%endmacro

%macro ___neq 0 ; ( a b -> q )
; Checks the top two elements of the stack
; for inequality and then collapses to either
; `1` or `0`.
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setne al
	movzx rax, al
%endmacro

%macro ___lt 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setl al
	movzx rax, al
%endmacro

%macro ___lte 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setle al
	movzx rax, al
%endmacro

%macro ___gt 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setg al
	movzx rax, al
%endmacro

%macro ___gte 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setge al
	movzx rax, al
%endmacro





