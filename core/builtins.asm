; UTILITIES
d_23: ; #
___count: ; ( cont -> x )
; Pushes the stack element count to the stack.
	pop r10
	mov rax, r9  ; `r9` is the current marker.
	sub rax, rsp
	shr rax, 3   ; Divide by `8` because we're using quad words on x86-64.
	push rax
	jmp r10

d_636c656172: ; clear
___clear: ; ( ... cont -> )
; Empty the stack.
	pop r10
	mov rsp, r9  ; Set the stack pointer to the current marker.
	jmp r10


; CONTROL FLOW
d_5b: ; [
___mark: ; ( cont -> | mark )
; Store the current base pointer value to the stack
; and then set the base pointer to the current stack
; pointer.
	pop r10
	push r9
	pop rax
	add rbp, 8
	mov [rbp], rax
	mov r9, rsp
	jmp r10

d_5d: ; ]
___unmark: ; ( mark cont -> )
; Set the marker to the value on top of the stack.
	pop r10
	push qword [rbp]
	sub rbp, 8
	pop r9
	jmp r10

d_3f: ; ?
___choose: ; ( cond t f cont -> q )
; Checks the condition and collapses to either
; `t` or `f` based on its value.
	pop r10
	pop rax ; f
	pop rbx ; t
	pop rcx ; cond

	cmp rcx, 1
	cmove rax, rbx  ; Move the the `true` case to `rax` if `rcx` is `1`.
	push rax
	jmp r10


; STACK OPERATIONS
d_3e7c: ; >|
___deque_push: ; ( cont -> )
; Push a value to the opposite side of the deque.
	pop r10
	pop rax
	add rbp, 8
	mov [rbp], rax
	jmp r10

d_7c3e: ; |>
___deque_pop: ; ( cont -> )
; Pull a value from the other side of the deque.
	pop r10
	push qword [rbp]
	sub rbp, 8
	jmp r10

d_706f70: ; pop
___pop: ; ( val cont -> )
; Pop the top element of the stack.
	pop r10
	pop rax
	jmp r10

d_737770: ; swp
___swp: ; ( a b cont -> b a )
; Swap the top two elements of the stack.
	pop r10
	pop rax ; b
	pop rbx ; a
	push rax ; b
	push rbx ; a
	jmp r10

d_647570: ; dup
___dup: ; ( x cont -> x x )
; Duplicate the top element of the stack.
	pop r10
	push qword [rsp]
	jmp r10

d_726f7472: ; rotr
___rotr: ; ( a b c cont -> c a b )
; Rotate the top three stack elements to the right.
	pop r10
	pop rax ; c
	pop rbx ; b
	pop rcx ; a
	push rax ; c
	push rcx ; a
	push rbx ; b
	jmp r10

d_676574: ; get
___get: ; ( i cont -> x )
; Copy the element at index `i` in the stack to the
; top of the stack.
; We shift `rax` left by 3 to multiply it by `8` and then
; offset the stack pointer by this value to get the address
; of the value.
	pop r10
	pop rax
	shl rax, 3
	push qword [rsp + rax]
	jmp r10

d_736574: ; set
___set: ; ( x i cont -> )
; Sets index `i` in the stack to value `x`.
; We multiply the given index by `8` and then access
; from the top of the stack.
	pop r10
	pop rax
	pop rbx
	shl rax, 3
	mov [rsp + rax], rbx
	jmp r10

d_6d676574: ; mget
___mget: ; ( addr cont -> x )
; Retrieve a value at a given memory address and push
; it to the stack.
	pop r10
	pop rax
	push qword [rax]
	jmp r10

d_6d736574: ; mset
___mset: ; ( x addr cont -> )
; Set the value at a given memory address to the value `x`.
	pop r10
	pop rax
	pop rbx
	mov [rax], rbx
	jmp r10


; ARITHMETIC
d_2b: ; +
___add: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	add rax, rbx
	push rax
	jmp r10

d_2d: ; -
___sub: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	sub rax, rbx
	push rax
	jmp r10

d_2a: ; *
___mul: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	imul rax, rbx
	push rax
	jmp r10

d_2f: ; /
___div: ; ( a b -> q )
; We need to sign extend `rax` to `rdx` because
; `idiv` operates on 128 bits.
	pop rax
	pop rbx
	cqo
	idiv rbx
	push rax

d_5c: ; %
___mod: ; ( a b cont -> q )
; Remainder is mostly the same as division on x86 but
; here we push the remainder which is stored in `rdx`.
	pop r10
	pop rax
	pop rbx
	cqo
	idiv rbx
	push rdx
	jmp r10

d_3c3c: ; <<
___lsh: ; ( a b cont -> q )
	pop r10
	pop rcx
	sal rax, cl
	jmp r10

d_3e3e: ; >>
___rsh: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rcx
	sar rax, cl
	push rax
	jmp r10

d_6e6567: ; neg
___neg: ; ( x cont -> y )
; Make the top element negative (two's complement).
	pop r10
	neg qword [rsp]
	jmp r10

d_616273: ; abs
___abs: ; ( x cont -> y )
; Return the absolute value of the top stack element.
	pop r10
	pop rax
	mov rbx, rax
	neg rax
	cmovl rax, rbx
	push rax
	jmp r10


; BITWISE
; These operations work on individual bits.
d_616e64: ; and
___and: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	and rax, rbx
	push rax
	jmp r10

d_6f72: ; or
___or: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	or rax, rbx
	push rax
	jmp r10

d_786f72: ; xor
___xor: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	xor rax, rbx
	push rax
	jmp r10

d_6e6f74: ; not
___not: ; ( x cont -> q )
	pop r10
	not qword [rsp]
	jmp r10


; COMPARISONS
d_3d: ; =
___eq: ; ( a b cont -> q )
; Checks the top two elements of the stack
; for equality and then collapses to either
; `1` or `0`.
	pop r10
	pop rax
	pop rbx
	cmp rax, rbx
	sete al
	movzx rax, al
	push rax
	jmp r10

d_213d: ; !=
___neq: ; ( a b cont -> q )
; Checks the top two elements of the stack
; for inequality and then collapses to either
; `1` or `0`.
	pop r10
	pop rax
	pop rbx
	cmp rax, rbx
	setne al
	movzx rax, al
	push rax
	jmp r10

d_3c: ; <
___lt: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	cmp rax, rbx
	setl al
	movzx rax, al
	push rax
	jmp r10

d_3c3d: ; <=
___lte: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	cmp rax, rbx
	setle al
	movzx rax, al
	push rax
	jmp r10

d_3e: ; >
___gt: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	cmp rax, rbx
	setg al
	movzx rax, al
	push rax
	jmp r10

d_3e3d: ; >=
___gte: ; ( a b cont -> q )
	pop r10
	pop rax
	pop rbx
	cmp rax, rbx
	setge al
	movzx rax, al
	push rax
	jmp r10

