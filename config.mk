# deck version (ISO 8601)
VERSION=2022-06-23

PREFIX=/usr/local
MANPREFIX=$(PREFIX)/share/man

INC=
LIB=

ASM=nasm
LD=ld

DECK_FLAGS=-O0 -felf64
DECK_LDFLAGS=-s -n --gc-sections $(LIB) $(LDFLAGS)

