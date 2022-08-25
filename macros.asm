; MACROS

; `rax` is the top element of the stack and these
; macros handle the boilerplate of shifting things into
; either `rax` or the actual stack.

; `rax` = top of stack
; `rbp` = base of stack


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
	mov rax, [rsp + 8]
%endmacro

%macro m_nip 0 ; ( a b -> b )
; Remove the element underneath the top element.
	sub rsp, 8
%endmacro

%macro m_clear 0 ; ( ... -> )
; Empty the stack.
	mov rsp, rbp
	mov rax, 0
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
	xchg rax, rbx
	add rax, rbx
%endmacro

%macro m_sub 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	sub rax, rbx
%endmacro

%macro m_mul 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	imul rax, rbx
%endmacro

%macro m_div 0 ; ( a b -> q )
; We need to sign extend `rax` to `rdx` because
; `idiv` operates on 128 bits.
	pop rbx
	xchg rax, rbx
	cqo
	idiv rbx
%endmacro

%macro m_mod 0 ; ( a b -> q )
; Remainder is mostly the same as division on x86 but
; here we move the remainder (stored in `rdx`) into `rax`
; which is the top of the stack.
	pop rbx
	xchg rax, rbx
	cqo
	idiv rbx
	mov rax, rdx
%endmacro

%macro m_lsh 0 ; ( a b -> q )
	pop rcx
	xchg rax, rcx
	sal rax, cl
%endmacro

%macro m_rsh 0 ; ( a b -> q )
	pop rcx
	xchg rax, rcx
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
	mov rbx, rax
	neg rax
	cmovl rax, rbx
%endmacro


; LOGICAL
; These operations work on booleans.
%macro m_and 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	test rax, rax
	setne al
	test rbx, rbx
	setne bl
	and al, bl
	movzx rax, al
%endmacro

%macro m_or 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	or rax, rbx
	setne al
	movzx rax, al
%endmacro

%macro m_xor 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
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
	xchg rax, rbx
	and rax, rbx
%endmacro

%macro m_bor 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	or rax, rbx
%endmacro

%macro m_bxor 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
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
	xchg rax, rbx
	cmp rax, rbx
	sete al
	movzx rax, al
%endmacro

%macro m_neq 0 ; ( a b -> q )
; Checks the top two elements of the stack
; for inequality and then collapses to either
; `1` or `0`.
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setne al
	movzx rax, al
%endmacro

%macro m_lt 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setl al
	movzx rax, al
%endmacro

%macro m_lte 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setle al
	movzx rax, al
%endmacro

%macro m_gt 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setg al
	movzx rax, al
%endmacro

%macro m_gte 0 ; ( a b -> q )
	pop rbx
	xchg rax, rbx
	cmp rax, rbx
	setge al
	movzx rax, al
%endmacro


; UTILITIES
%macro m_count 0 ; ( -> x )
; Pushes the stack element count to the stack.
	mov rbx, rbp
	sub rbx, rsp
	shr rbx, 3   ; Divide by `8`.
	push rax
	mov rax, rbx
%endmacro

%macro m_choose 0 ; ( cond t f -> q )
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

%macro m_here 0 ; ( -> x )
; Pushes the current instruction pointer to the stack.
	push rax
	lea rax, [rel + $+0]
%endmacro

%macro m_go 0 ; ( x -> )
; Jumps to the address on the top of the stack.
	mov rbx, rax
	pop rax
	jmp rbx
%endmacro

%macro m_if 0 ; ( cond x -> )
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

%macro m_exit 0 ; ( x -> )
; Exit syscall.
; We overwrite the top of stack here but we're exiting so who cares.
	mov rdi, rax
	mov rax, 60
	syscall
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

%macro m_hex 0 ; ( x -> )
; Writes a quad word to stdout in hex format without
; leading zeroes.
; We take one nibble at a time from the value at the
; top of the stack in `rax`. We subtract `10` from the
; nibble in order to overflow and shift the values `0`-`9`
; up to `0xf5`-`0xff`. Doing this lets us check the first bit
; to see if this is a numeric digit (`0`-`9`) or an alphabetic
; digit (`a`-`f`). We shift the literal `0x27` right by this
; value so effectively it evaluates to either `0x27` or `0`.
; We can then add this value on top of adding `0x3a` to bring
; everything into the appropriate ASCII ranges and that gives
; us our character.
	mov r8, rsp ; Save stack position.
	push 10  ; \n

	%%take:
	mov rbx, rax ; n = (c & 0x0f) - 10
	and rbx, 0h0f
	sub rbx, 10

	mov rcx, rbx  ; x = 0x27 >> ((n & 0x80) >> 4)
	and rcx, 0h80
	shr rcx, 4
	mov rdx, 0h27
	shr rdx, cl

	lea rbx, [rbx + rdx + 0x3a] ; n + 0x3a + x

	push bx

	shr rax, 4
	cmp rax, 0
	jnz %%take

	mov rax, 1   ; write
	mov rdi, 1   ; stdout
	mov rdx, r8  ; count
	sub rdx, rsp
	mov rsi, rsp ; source
	syscall

	mov rsp, r8 ; Restore the stack to before we wrote the characters.
	pop rax ; Move up the next value in the stack.
%endmacro

%macro m_min 0 ; ( a b -> q )
; Collapses to the minimum of the top two stack
; values.
	pop rbx
	cmp rax, rbx
	cmovg rax, rbx
%endmacro

%macro m_max 0 ; ( a b -> q )
; Collapses to the maximum of the top two stack
; values.
	pop rbx
	cmp rax, rbx
	cmovl rax, rbx
%endmacro


; BOILERPLATE
%macro m_header 0
	mov rbp, rsp
	mov rax, 0
%endmacro

%macro m_footer 0
	mov rax, 60
	xor rdi, rdi
	syscall
%endmacro

