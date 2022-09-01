; STACK OPERATIONS


%macro ___push 1 ; ( x -> y )
; Move a literal into `rax` after pushing
; the old top of stack.
	push rax
	mov rax, %1
%endmacro

%macro ___pop 0 ; ( x -> )
; Pop the top element of the stack.
; Popping to `rax` implicitly overwrites the top element.
	pop rax
%endmacro

%macro ___swap 0 ; ( a b -> b a )
; Swap the top two elements of the stack.
	xchg rax, [rsp]
%endmacro

%macro ___dup 0 ; ( x -> x x )
; Duplicate the top element of the stack.
; This is fairly trivial to do by just pushing
; `rax` since the value currently in `rax` will
; be preserved after the push.
	push rax
%endmacro

%macro ___over 0 ; ( a b -> a b a )
; Copy the element over the top element to the top
; of the stack.
	push rax
	mov rax, [rsp + 8]
%endmacro

%macro ___nip 0 ; ( a b -> b )
; Remove the element underneath the top element.
	sub rsp, 8
%endmacro

%macro ___rotl 0 ; ( a b c -> b c a )
; Rotate the top three stack elements to the left.
	pop rbx
	pop rcx
	xchg rcx, rax
	push rbx
	push rcx
%endmacro

%macro ___rotr 0 ; ( a b c -> c a b )
; Rotate the top three stack elements to the right.
	pop rbx
	pop rcx
	push rax
	push rcx
	mov rax, rbx
%endmacro

%macro ___get 0 ; ( x -> y )
; Copy the element at index `x` in the stack to the
; top of the stack.
; We shift `rax` left by 3 to multiply it by `8` and then
; offset the stack pointer by this value to get the address
; of the value.
	shl rax, 3
	mov rax, [rsp + rax]
%endmacro

%macro ___set 0 ; ( x i -> )
; Sets index `i` in the stack to value `x`.
; We pop the value to set and then multiply the
; index by `8`. We then store `x` at this offset
; before finally popping the next value in the stack
; to `rax`.
	pop rbx
	shl rax, 3
	mov [rsp + rax], rbx
	pop rax
%endmacro

%macro ___mget 0 ; ( m -> x)
; Retrieve a value at a given memory address and push
; it to the stack.
	mov rax, [rax]
%endmacro

%macro ___mset 0 ; ( x m -> )
; Set the value at a given memory address to the value `x`.
	pop rbx
	mov [rax], rbx
	pop rax
%endmacro

%macro ___deque_push 0 ; ( -> )
; Push a value to the opposite side of the stack.
	mov [rbp], rax
	add rbp, 8
	pop rax
%endmacro

%macro ___deque_pop 0 ; ( -> )
; Push a value to the opposite side of the stack.
	push rax
	sub rbp, 8
	mov rax, [rbp]
%endmacro

; %macro ___mov 0 ; ( x -> y )
; ; Copy the element at index `x` in the stack to the
; ; top of the stack.
; ; We shift `rax` left by 3 to multiply it by `8` and then
; ; offset the stack pointer by this value to get the address
; ; of the value.
; 	shl rax, 3
; 	mov rax, [rsp + rax]
; %endmacro



