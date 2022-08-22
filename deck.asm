%include "macros.asm"

bits 64
global _start

; CODE
section .text
_start:
m_begin

	m_push 0h11223344aabbccdd
	m_hex

m_end

