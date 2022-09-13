# deck
.POSIX:

include config.mk

all: deck

deck.o: deck.dk config.mk Makefile
	./dcc < $< > deck.asm
	$(ASM) $(DECK_FLAGS) deck.asm -o deck.o

deck: deck.o
	$(LD) -o $@ deck.o $(DECK_LDFLAGS)

dis:
	objdump --insn-width=15 --visualize-jumps=color -w -M intel -d deck

clean:
	rm -rf deck deck.o deck.asm deck.lst deck-$(VERSION).tar.gz

dist: clean deck
	mkdir -p deck-$(VERSION)
	cp -R LICENSE Makefile README.md config.mk deck.dk deck.1 \
		deck-$(VERSION)
	tar -cf - deck-$(VERSION) | gzip > deck-$(VERSION).tar.gz
	rm -rf deck-$(VERSION)

install: deck
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f deck $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/deck
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < deck.1 > $(DESTDIR)$(MANPREFIX)/man1/deck.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/deck.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/deck
	rm -f $(DESTDIR)$(MANPREFIX)/man1/deck.1

.PHONY: all dis clean dist install uninstall

