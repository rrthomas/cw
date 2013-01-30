dist_man_MANS = cw.1 cwe.1

bin_PROGRAMS = cw
dist_bin_SCRIPTS = colorcfg

cw_SOURCES = cw.c cw.h

EXTRA_DIST = $(srcdir)/def/*.in

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

install-exec-hook:
	cd $(DESTDIR)$(bindir) && rm -f cwe$(EXEEXT) && $(LN_S) cw$(EXEEXT) cwe$(EXEEXT)

uninstall-local:
	rm -rf $(DESTDIR)$(bindir)/cwe$(EXEEXT) $(DESTDIR)$(pkglibdir)
