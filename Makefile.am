SUBDIRS = lib
ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)

dist_man_MANS = cw.1

bin_PROGRAMS = cw

cw_SOURCES = cw.c
cw_LDADD = lib/lib$(PACKAGE).a $(LIBINTL) $(PTY_LIB)

EXTRA_DIST = $(srcdir)/def/*.in m4/gnulib-cache.m4

clean-local:
	for FILE in def/*.in; do \
	  rm -f $${FILE%.in};\
	done

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(pkglibdir)/
	$(MKDIR_P) def
	for i in $(srcdir)/def/*.in; do \
	  out=$${i#$(srcdir)/}; \
	  out=$${out%.in}; \
	  printf "#!$(bindir)/cw\n" > "$$out"; \
	  cat $$i >> "$$out"; \
	  $(INSTALL) $$out $(DESTDIR)$(pkglibdir)/; \
	done

uninstall-local:
	rm -rf $(DESTDIR)$(pkglibdir)
