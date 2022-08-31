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

%macro ___at 0 ; ( x -> y )
; Copy the element at index `x` in the stack to the
; top of the stack.
; We shift `rax` left by 3 to multiply it by `8` and then
; offset the stack pointer by this value to get the address
; of the value.
	shl rax, 3
	mov rax, [rsp + rax]
%endmacro

%macro ___mv 0 ; ( x -> y )
; Copy the element at index `x` in the stack to the
; top of the stack.
; We shift `rax` left by 3 to multiply it by `8` and then
; offset the stack pointer by this value to get the address
; of the value.
	shl rax, 3
	mov rax, [rsp + rax]
%endmacro



