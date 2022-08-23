%include "macros.asm"

bits 64
global _start

; CODE
section .text
_start:
m_begin

	m_push 0hcd
	m_push 0hab
	m_hex
	m_hex

m_end

