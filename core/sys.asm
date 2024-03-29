; UTILITIES
d_7379735f65786974:
sys_exit: ; ( x -> )
; Exit syscall.
; We overwrite the top of stack here but we're exiting so who cares.
	mov rax, 60
	pop rdi
	syscall

d_7379735f657272:
sys_err: ; ( -> )
; Exit with a `1` status to signify that something went wrong.
	mov rax, 60
	mov rdi, 1
	syscall

d_7379735f6f6b:
sys_ok: ; ( -> )
; Exit with `0` status to signify that the program
; performed correctly.
	mov rax, 60
	xor rdi, rdi
	syscall


; MEMORY
d_7379735f6d6d6170:
sys_mmap: ; ( n cont -> p )
	pop r13 ; cont
	pop rsi ; n
	push r9

	mov rax, 9  ; mmap
	mov rdi, 0  ; addr
	mov rdx, 3  ; prot
	mov r10, 34 ; flags
	mov r8, -1  ; fd
	mov r9, 0   ; offset
	syscall
	pop r9
	push rax    ; ptr

	jmp r13

d_7379735f756e6d6170:
sys_unmap: ; ( ptr n cont -> )
	mov rax, 11
	mov rdi, [rsp + 8]
	mov rsi, [rsp + 16]

	add rsp, 24
	jmp [rsp - 24]


; INPUT/OUTPUT
d_696f5f72656164:
io_read: ; ( cont -> x )
; Reads as many characters as will with into a single word.
; On x86-64, this is eight characters.
; First we save the return address to `rbx` because `rax`
; used in the syscall. We reserve some space on the stack
; for the read characters to be saved. We then pop this
; value to `rax` so it's at the top of the stack. We jump
; to the previously saved return address to continue
; execution.
	pop r10
	sub rsp, 8

	mov rax, 0   ; read
	mov rdi, 0   ; stdin
	mov rsi, rsp ; dest
	mov rdx, 8   ; count
	syscall

	jmp r10

d_696f5f7772697465:
io_write: ; ( x cont -> )
; Write the element on the top of the stack to stdout.
; We write a quad word worth of data which on x86-64 means
; eight bytes.
; We save the return address to `rbx` first of all and then
; perform the syscall on the stack pointer. We then remove the
; top element of the stack and move the next element up to `rax`
; before jumping to the return address.
	pop r10

	mov rax, 1   ; write
	mov rdi, 1   ; stdout
	mov rsi, rsp ; source
	mov rdx, 8   ; count
	syscall

	add rsp, 8
	jmp r10

d_696f5f6865786c6e:
io_hexln: ; ( x cont -> )
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
	pop r10
	pop rax
	mov r8, rsp   ; Save stack position.

	dec rsp  ; \n
	mov byte [rsp], 10

	.take:
	mov rbx, rax ; n = (c & 0x0f) - 10
	and rbx, 0h0f
	sub rbx, 10

	mov rcx, rbx  ; x = 0x27 >> ((n & 0x80) >> 4)
	and rcx, 0h80
	shr rcx, 4
	mov rdx, 0h27
	shr rdx, cl

	lea rbx, [rbx + rdx + 0x3a] ; n + 0x3a + x

	dec rsp  ; Push character.
	mov [rsp], bl

	shr rax, 4
	cmp rax, 0
	jnz .take

	mov rax, 1   ; write
	mov rdi, 1   ; stdout
	mov rdx, r8  ; count
	sub rdx, rsp
	mov rsi, rsp ; source
	syscall

	mov rsp, r8
	jmp r10


d_696f5f696e746c6e:
io_intln: ; ( x cont -> )
; Writes a quad word to stdout in decimal format without
; leading zeroes.
; The surrounding boilerplate functions much the same as
; `io_hexln` but the core of the algorithm is a bit different.
; We take advantage of the fact that x86 idiv computes both
; division and remainder together. We can simply add ASCII
; '0' to the remainder to get the digit to print and the
; number is divided by ten to move to the next digit.
	pop r10 ; cont
	pop rax ; x
	mov r8, rsp   ; Save stack position.
	mov rbx, 10

	dec rsp  ; \n
	mov byte [rsp], 10

	.take:
	cqo
	idiv rbx
	add rdx, 0h30

	dec rsp  ; Push character.
	mov [rsp], dl

	cmp rax, 0
	jnz .take

	mov rax, 1   ; write
	mov rdi, 1   ; stdout
	mov rdx, r8  ; count
	sub rdx, rsp
	mov rsi, rsp ; source
	syscall

	mov rsp, r8
	jmp r10



