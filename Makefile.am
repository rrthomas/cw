SUBDIRS = lib
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
AM_CPPFLAGS = $(ISYSTEM)$(srcdir)/lib -DSCRIPTSDIR=\"$(pkglibexecdir)\"

bin_PROGRAMS = cw
cw_SOURCES = cw.c
cw_LDADD = lib/lib$(PACKAGE).a $(LIBINTL) $(PTY_LIB) $(GETHOSTNAME_LIB) $(LIBSOCKET)

dist_man_MANS = cw.1
EXTRA_DIST = $(srcdir)/def/*.cw m4/gnulib-cache.m4
pkglibexec_SCRIPTS = $(basename $(wildcard def/*.cw))

%: %.cw
	printf "#!$(bindir)/cw\n" > $@
	cat $< >> $@
