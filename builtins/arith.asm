; ARITHMETIC
; Most of these operations work in the same way by
; first popping the top two elements, performing
; the operation and then pushing the result back to
; the stack.


; MATHS
%macro ___add 0 ; ( a b -> q )
	pop rax
	pop rbx
	add rax, rbx
	push rax
%endmacro

%macro ___sub 0 ; ( a b -> q )
	pop rax
	pop rbx
	sub rax, rbx
	push rax
%endmacro

%macro ___mul 0 ; ( a b -> q )
	pop rax
	pop rbx
	imul rax, rbx
	push rax
%endmacro

%macro ___div 0 ; ( a b -> q )
; We need to sign extend `rax` to `rdx` because
; `idiv` operates on 128 bits.
	pop rax
	pop rbx
	cqo
	idiv rbx
	push rax
%endmacro

%macro ___mod 0 ; ( a b -> q )
; Remainder is mostly the same as division on x86 but
; here we push the remainder which is stored in `rdx`.
	pop rax
	pop rbx
	cqo
	idiv rbx
	push rdx
%endmacro

%macro ___lsh 0 ; ( a b -> q )
	pop rcx
	sal rax, cl
%endmacro

%macro ___rsh 0 ; ( a b -> q )
	pop rax
	pop rcx
	sar rax, cl
	push rax
%endmacro

%macro ___inc 0 ; ( x -> y )
; Increment top element of stack by one.
	inc qword [rsp]
%endmacro

%macro ___dec 0 ; ( x -> y )
; Decrement top element of stack by one.
	dec qword [rsp]
%endmacro

%macro ___neg 0 ; ( x -> y )
; Make the top element negative (two's complement).
	neg qword [rsp]
%endmacro

%macro ___abs 0 ; ( x -> y )
; Return the absolute value of the top stack element.
	pop rax
	mov rbx, rax
	neg rax
	cmovl rax, rbx
	push rax
%endmacro

%macro ___min 0 ; ( a b -> q )
; Collapses to the minimum of the top two stack
; values.
	pop rax
	pop rbx
	cmp rax, rbx
	cmovg rax, rbx
	push rax
%endmacro

%macro ___max 0 ; ( a b -> q )
; Collapses to the maximum of the top two stack
; values.
	pop rax
	pop rbx
	cmp rax, rbx
	cmovl rax, rbx
	push rax
%endmacro


; LOGICAL
; These operations work on booleans.
%macro ___and 0 ; ( a b -> q )
	pop rax
	pop rbx
	test rax, rax
	setne al
	test rbx, rbx
	setne bl
	and al, bl
	movzx rax, al
	push rax
%endmacro

%macro ___or 0 ; ( a b -> q )
	pop rax
	pop rbx
	or rax, rbx
	setne al
	movzx rax, al
	push rax
%endmacro

%macro ___xor 0 ; ( a b -> q )
	pop rax
	pop rbx
	xor rax, rbx
	setne al
	movzx rax, al
	push rax
%endmacro

%macro ___not 0 ; ( x -> q )
	pop rax
	test rax, rax
	setne al
	movzx rax, al
	push rax
%endmacro


; BITWISE
; These operations work on individual bits.
%macro ___band 0 ; ( a b -> q )
	pop rax
	pop rbx
	and rax, rbx
	push rax
%endmacro

%macro ___bor 0 ; ( a b -> q )
	pop rax
	pop rbx
	or rax, rbx
	push rax
%endmacro

%macro ___bxor 0 ; ( a b -> q )
	pop rax
	pop rbx
	xor rax, rbx
	push rax
%endmacro

%macro ___bnot 0 ; ( x -> q )
	not qword [rsp]
%endmacro


; COMPARISONS
%macro ___eq 0 ; ( a b -> q )
; Checks the top two elements of the stack
; for equality and then collapses to either
; `1` or `0`.
	pop rax
	pop rbx
	cmp rax, rbx
	sete al
	movzx rax, al
	push rax
%endmacro

%macro ___neq 0 ; ( a b -> q )
; Checks the top two elements of the stack
; for inequality and then collapses to either
; `1` or `0`.
	pop rax
	pop rbx
	cmp rax, rbx
	setne al
	movzx rax, al
	push rax
%endmacro

%macro ___lt 0 ; ( a b -> q )
	pop rax
	pop rbx
	cmp rax, rbx
	setl al
	movzx rax, al
	push rax
%endmacro

%macro ___lte 0 ; ( a b -> q )
	pop rax
	pop rbx
	cmp rax, rbx
	setle al
	movzx rax, al
	push rax
%endmacro

%macro ___gt 0 ; ( a b -> q )
	pop rax
	pop rbx
	cmp rax, rbx
	setg al
	movzx rax, al
	push rax
%endmacro

%macro ___gte 0 ; ( a b -> q )
	pop rax
	pop rbx
	cmp rax, rbx
	setge al
	movzx rax, al
	push rax
%endmacro





