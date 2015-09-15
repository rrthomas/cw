# Top-level Makefile
#
# Copyright (c) 2013-2015 Reuben Thomas <rrt@sc3d.org>
#
# This file is part of cw.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

PACKAGE=cw
VERSION=2.0.5

LUA ?= lua

prefix ?= /usr/local
exec_prefix=${prefix}
top_srcdir=.
srcdir=.
top_builddir=.
bindir=${exec_prefix}/bin
mandir=${prefix}/share/man
pkglibexecdir=${exec_prefix}/libexec/$(PACKAGE)

bin_SCRIPTS = cw cw-definitions-path $(top_srcdir)/script/*
man_MANS = $(PACKAGE).1
pkglibexec_SCRIPTS = $(basename $(wildcard def/*.lua))

all: $(bin_SCRIPTS) $(man_MANS) $(pkglibexec_SCRIPTS)

$(pkglibexec_SCRIPTS): Makefile

%: %.lua
	printf "#!$(bindir)/$(PACKAGE)\n" > $@
	cat $< >> $@

edit = sed \
	-e 's|@LUA[@]|$(LUA)|g' \
	-e 's|@PACKAGE[@]|$(PACKAGE)|g' \
	-e 's|@VERSION[@]|$(VERSION)|g' \
	-e 's|@abs_srcdir[@]|$(abs_srcdir)|g' \
	-e 's|@pkglibexecdir[@]|$(pkglibexecdir)|g'

$(top_builddir)/$(PACKAGE): $(PACKAGE).in Makefile
	rm -f $@ $@.tmp
	$(edit) '$@.in' >$@.tmp
	mv $@.tmp $@
	chmod +x $@
	$(LUA) $@ --version >/dev/null || ( cat $@ && rm $@ && false )

$(top_builddir)/$(PACKAGE)-definitions-path: $(PACKAGE)-definitions-path.in Makefile
	rm -f $@ $@.tmp
	$(edit) '$@.in' >$@.tmp
	mv $@.tmp $@
	chmod +x $@
	$(LUA) $@ --version >/dev/null || ( cat $@ && rm $@ && false )

$(top_builddir)/luarocks-config.lua: luarocks-config.lua.in Makefile
	rm -f $@ $@.tmp
	$(edit) '$@.lua.in' >$@.tmp
	mv $@.tmp $@

$(PACKAGE).1: $(srcdir)/$(PACKAGE).1.in Makefile
	rm -f $@ $@.tmp
	$(edit) $(srcdir)/$(PACKAGE).1.in >$@.tmp
	mv $@.tmp $@

install:
	mkdir -p $(bindir) $(pkglibexecdir) $(mandir)/man1
	cp $(pkglibexec_SCRIPTS) $(pkglibexecdir)
	chmod 755 $(pkglibexecdir)/*
	cp $(bin_SCRIPTS) $(bindir)
	chmod 755 $(bindir)/*
	cp $(man_MANS) $(mandir)/man1
	chmod 644 $(mandir)/man1/*

clean:
	rm -f $(pkglibexec_SCRIPTS) $(PACKAGE) $(PACKAGE)-definitions-path

rockspecs:
	rm -f *.rockspec
	for v in $(VERSION) git; do \
		$(LUA) mkrockspecs.lua $(PACKAGE) $$v; \
	done

tag-release:
	git diff --exit-code && \
	git tag -a -m "Release tag" v$(VERSION) && \
	git push && git push --tags

release: rockspecs
	$(MAKE) tag-release && \
	LUAROCKS_CONFIG=`pwd`/luarocks-config.lua luarocks --tree=`pwd`/luarocks build $(PACKAGE)-$(VERSION)-1.rockspec && \
	woger luarocks package=$(PACKAGE) version=$(VERSION)
