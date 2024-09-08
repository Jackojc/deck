# cdc version
VERSION = 2024-08-30

# prefixes
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

PKG_CONFIG = pkg-config

# headers/libs
INCS = -Iinclude/
LIBS = -lgccjit
DEPS =

# flags
CFLAGS += -Wall -Wextra -Wno-unused -O3 -g -std=c11

CFLAGS += $(foreach dep, $(DEPS), $(shell pkg-config --cflags $(dep)))
LDFLAGS += $(foreach dep, $(DEPS), $(shell pkg-config --libs $(dep)))

DECK_CPPFLAGS = -DVERSION=\"$(VERSION)\"
DECK_CFLAGS = $(INCS) $(DECK_CPPFLAGS) $(CPPFLAGS) $(CFLAGS)
DECK_LDFLAGS = $(LIBS) $(LDFLAGS)
