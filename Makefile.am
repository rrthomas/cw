SUBDIRS = lib
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = consolewrap.la

consolewrap_la_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
consolewrap_la_CPPFLAGS = $(ISYSTEM)$(srcdir)/lib $(LUA_INCLUDE) -DSCRIPTSDIR=\"$(pkglibexecdir)\"
consolewrap_la_SOURCES = lcw.c
consolewrap_la_LDFLAGS = -module -avoid-version $(LIBINTL) $(PTY_LIB) $(GETHOSTNAME_LIB) $(LIBSOCKET) lib/libgnu.la

bin_SCRIPTS = cw

man_MANS = cw.1
EXTRA_DIST = $(srcdir)/def/*.lua m4/gnulib-cache.m4 $(PACKAGE).1.in
DISTCLEANFILES = $(PACKAGE).1
pkglibexec_SCRIPTS = $(basename $(wildcard def/*.lua))

$(pkglibexec_SCRIPTS): config.status

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
