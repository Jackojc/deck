# deck
.POSIX:

include config.mk

all: deck

deck.o: deck.asm
	$(ASM) $(DECK_FLAGS) $< -o deck.o

deck: deck.o
	$(LD) -o $@ deck.o $(DECK_LDFLAGS)

clean:
	rm -rf deck deck.o deck-$(VERSION).tar.gz

dist: clean deck
	mkdir -p deck-$(VERSION)
	cp -R LICENSE Makefile README.md config.mk deck.asm deck.1 \
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

.PHONY: all clean dist install uninstall

