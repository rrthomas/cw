# Top-level Makefile.am
#
# Copyright (c) 2013 Reuben Thomas <rrt@sc3d.org>
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

ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS = cw

man_MANS = cw.1
EXTRA_DIST = $(srcdir)/def/*.lua $(PACKAGE).1.in
DISTCLEANFILES = $(PACKAGE).1
pkglibexec_SCRIPTS = $(basename $(wildcard def/*.lua))

$(pkglibexec_SCRIPTS): config.status

edit = sed \
	-e 's|@pkglibexecdir[@]|$(pkglibexecdir)|g'

$(top_builddir)/cw: cw.lua Makefile
	rm -f $@ $@.tmp
	$(edit) '$@.lua' >$@.tmp
	mv $@.tmp $@
	chmod +x $@
	$(LUA) $(top_builddir)/cw --version >/dev/null || ( cat $@ && rm $@ )

%: %.lua
	printf "#!$(bindir)/cw\n" > $@
	cat $< >> $@

clean-local:
	rm -f $(pkglibexec_SCRIPTS) cw

$(PACKAGE).1: $(srcdir)/$(PACKAGE).1.in Makefile.am config.status
	rm -f $@ $@.tmp
	$(edit) $(srcdir)/$(PACKAGE).1.in >$@.tmp
	mv $@.tmp $@
