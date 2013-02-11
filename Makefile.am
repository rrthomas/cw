SUBDIRS = lib
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
AM_CPPFLAGS = $(ISYSTEM)$(srcdir)/lib $(LUA_INCLUDE) -DSCRIPTSDIR=\"$(pkglibexecdir)\"

bin_PROGRAMS = cw
cw_SOURCES = cw.c
cw_LDADD = lib/lib$(PACKAGE).a $(LIBINTL) $(PTY_LIB) $(GETHOSTNAME_LIB) $(LIBSOCKET) $(LUA_LIB)

man_MANS = cw.1
EXTRA_DIST = $(srcdir)/def/*.lua m4/gnulib-cache.m4 $(PACKAGE).1.in
DISTCLEANFILES = $(PACKAGE).1
pkglibexec_SCRIPTS = $(basename $(wildcard def/*.lua))

%: %.lua
	printf "#!$(bindir)/cw\n" > $@
	cat $< >> $@

clean-local:
	rm -f $(pkglibexec_SCRIPTS)

edit = sed \
	-e 's|@pkglibexecdir[@]|$(pkglibexecdir)|g'

$(PACKAGE).1: $(srcdir)/$(PACKAGE).1.in Makefile.am config.status
	rm -f $@ $@.tmp
	$(edit) $(srcdir)/$(PACKAGE).1.in >$@.tmp
	mv $@.tmp $@
