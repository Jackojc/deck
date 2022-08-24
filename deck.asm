%include "macros.asm"
bits 64
global _start
section .text
_start:
m_header
main:
m_fn_begin
m_push 1
m_push 2
m_add
m_push 3
m_mul
m_fn_end
foo:
m_fn_begin
m_fn_end
m_footer
