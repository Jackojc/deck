%include "macros.asm"

bits 64
global _start

; CODE
section .text
_start:
m_begin

	m_push foo
	m_push 0
	m_if

	m_push 0haa
	m_hex
	m_exit 0

	foo:
	m_push 0hff
	m_hex

m_end

