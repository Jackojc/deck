CC?=gcc
DBG?=gdb

TARGET?=debug

CSRC_BIN:=$(wildcard src/*.c)
COBJ_BIN:=$(patsubst src/%.c, build/%.c.o, $(CSRC_BIN))

CFLAGS+=-Wall -Wextra  -Werror -c -MMD
LFLAGS+=-lm

ifeq ($(TARGET), debug)
CFLAGS+=-Og -g -fsanitize=undefined -fstack-protector-strong
LFLAGS+=-fsanitize=undefined -fstack-protector-strong
else ifeq ($(TARGET), release)
CFLAGS+=-O2 
else
$(error "unknown target $(TARGET)")
endif

DEPS:=

CFLAGS+=$(foreach dep, $(DEPS), $(shell pkg-config --cflags $(dep)))
LFLAGS+=$(foreach dep, $(DEPS), $(shell pkg-config --libs $(dep)))

CFLAGS_BIN:=$(CFLAGS) -Iinclude/ 

BIN:=build/cdc

build/%.c.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BIN) $< -o $@

$(BIN): $(COBJ_BIN)
	$(CC) $(LFLAGS) $(COBJ_BIN)  -o $@

.PHONY: all lib base run debug clean

all: $(BIN)

run: $(BIN)
	./$(BIN) 

debug: $(BIN)
	$(DBG) $(BIN)

clean: 
	rm -rf build/

-include build/*.c.d 
