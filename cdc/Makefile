# cdc

.POSIX:

include config.mk

BIN = build/cdc

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.c.o, $(SRC))

all: build/cdc

setup:
	@mkdir -p build/

build/%.c.o: src/%.c setup
	$(CC) $(DECK_CFLAGS) -c $< -o $@

build/cdc: $(OBJ)
	$(CC) -o $@ $(OBJ) $(DECK_LDFLAGS)

clean:
	rm -rf build/ cdc-$(VERSION).tar.gz

install: cdc
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f build/cdc $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/cdc
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < cdc.1 > $(DESTDIR)$(MANPREFIX)/man1/cdc.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/cdc.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/cdc
	rm -f $(DESTDIR)$(MANPREFIX)/man1/cdc.1

.PHONY: all clean install uninstall
