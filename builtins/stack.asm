; STACK OPERATIONS

%macro ___push 1 ; ( -> val )
; Push literal to the deque.
	push qword %1
%endmacro

%macro ___deque_push 0 ; ( -> )
; Push a value to the opposite side of the deque.
	pop rax
	add rbp, 8
	mov [rbp], rax
%endmacro

%macro ___deque_pop 0 ; ( -> )
; Pull a value from the other side of the deque.
	push qword [rbp]
	sub rbp, 8
%endmacro

%macro ___pop 0 ; ( val -> )
; Pop the top element of the stack.
	pop rax
%endmacro

%macro ___swap 0 ; ( a b -> b a )
; Swap the top two elements of the stack.
	pop rax ; b
	pop rbx ; a
	push rax ; b
	push rbx ; a
%endmacro

%macro ___dup 0 ; ( x -> x x )
; Duplicate the top element of the stack.
	push qword [rsp]
%endmacro

%macro ___over 0 ; ( a b -> a b a )
; Copy the element over the top element to the top
; of the stack.
	___push [rsp + 8]
%endmacro

%macro ___nip 0 ; ( a b -> b )
; Remove the element underneath the top element.
	pop rax
	pop rbx
	push rax
%endmacro

%macro ___tuck 0 ; ( a b -> b a b )
; Copy the top of the stack under the second element
; in the stack.
	pop rax ; b
	pop rbx ; a
	push rax ; b
	push rbx ; a
	push rax ; b
%endmacro

%macro ___rotl 0 ; ( a b c -> b c a )
; Rotate the top three stack elements to the left.
	pop rax ; c
	pop rbx ; b
	pop rcx ; a
	push rbx ; b
	push rax ; c
	push rcx ; a
%endmacro

%macro ___rotr 0 ; ( a b c -> c a b )
; Rotate the top three stack elements to the right.
	pop rax ; c
	pop rbx ; b
	pop rcx ; a
	push rax ; c
	push rcx ; a
	push rbx ; b
%endmacro

%macro ___get 0 ; ( i -> x )
; Copy the element at index `i` in the stack to the
; top of the stack.
; We shift `rax` left by 3 to multiply it by `8` and then
; offset the stack pointer by this value to get the address
; of the value.
	pop rax
	shl rax, 3
	___push [rsp + rax]
%endmacro

%macro ___set 0 ; ( x i -> )
; Sets index `i` in the stack to value `x`.
; We multiply the given index by `8` and then access
; from the top of the stack.
	pop rax
	pop rbx
	shl rax, 3
	mov [rsp + rax], rbx
%endmacro

%macro ___mget 0 ; ( addr -> x )
; Retrieve a value at a given memory address and push
; it to the stack.
	pop rax
	___push [rax]
%endmacro

%macro ___mset 0 ; ( x addr -> )
; Set the value at a given memory address to the value `x`.
	pop rax
	pop rbx
	mov [rax], rbx
%endmacro

