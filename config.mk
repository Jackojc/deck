# deck version (ISO 8601)
VERSION=2022-06-23

PREFIX=/usr/local
MANPREFIX=$(PREFIX)/share/man

INC=
LIB=

ASM=nasm
LD=ld

DBG=yes

ifeq ($(DBG),no)
	DECK_FLAGS=-O0 -felf64
	DECK_LDFLAGS=-s -n --gc-sections $(LIB) $(LDFLAGS)
else ifeq ($(DBG),yes)
	DECK_FLAGS=-O0 -felf64 -F dwarf -g
	DECK_LDFLAGS=-n --gc-sections $(LIB) $(LDFLAGS)
else
$(error DBG should be either yes or no)
endif

