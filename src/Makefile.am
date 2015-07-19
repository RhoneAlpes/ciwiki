PREFIXDIR  = $(prefix)
PKGDATADIR = $(datadir)/matchbox
DATADIR    = $(datadir)

AM_CFLAGS = $(GCC_FLAGS)

INCLUDES = -DDATADIR=\"$(DATADIR)\" -DPKGDATADIR=\"$(PKGDATADIR)\" -DPREFIX=\"$(PREFIXDIR)\"

bin_PROGRAMS = ciwiki

ciwiki_SOURCES = ci.c  ci.h  http.c  http.h  \
                   util.c  util.h  wiki.c  wiki.h  \
                   wikilogin.c  wikilogin.h  wikitext.h \
                   wikichanges.c  wikichanges.h \
                   wikiashtml.c  wikiashtml.h \
                   wikientries.c  wikientries.h

