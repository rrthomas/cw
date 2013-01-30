man_MANS = cw.1 cwe.1

bin_PROGRAMS = cw
dist_bin_SCRIPTS = colorcfg

cw_SOURCES = cw.c

clean-local:
	for FILE in def/*.in; do \
	  rm -f $${FILE%.in};\
	done

install-data-local:
	$(MKDIR_P) $(pkglibdir)/
	for FILE in def/*.in; do \
	  printf "#!$(bindir)/cw\n" > $${FILE%.in};\
	  cat $$FILE >> $${FILE%.in};\
	  $(INSTALL) $${FILE%.in} $(pkglibdir)/;\
	done

install-exec-local:
	cd $(bindir) && rm cwe$(EXEEXT) && $(LN_S) cw$(EXEEXT) cwe$(EXEEXT)

uninstall-local:
	rm -rf $(bindir)/cwe$(EXEEXT) $(pkglibdir)/cw
