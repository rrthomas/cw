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
man_MANS = $(PACKAGE).1

EXTRA_DIST = $(srcdir)/def/*.lua $(PACKAGE).1.in
DISTCLEANFILES = $(man_MANS)

pkglibexec_SCRIPTS = $(basename $(wildcard def/*.lua))
$(pkglibexec_SCRIPTS): config.status

%: %.lua
	printf "#!$(bindir)/$(PACKAGE)\n" > $@
	cat $< >> $@

edit = sed \
	-e 's|@pkglibexecdir[@]|$(pkglibexecdir)|g'

$(top_builddir)/$(PACKAGE): $(PACKAGE).lua Makefile
	rm -f $@ $@.tmp
	$(edit) '$@.lua' >$@.tmp
	mv $@.tmp $@
	chmod +x $@
	$(LUA) $@ --version >/dev/null || ( cat $@ && rm $@ )

$(PACKAGE).1: $(srcdir)/$(PACKAGE).1.in Makefile.am config.status
	rm -f $@ $@.tmp
	$(edit) $(srcdir)/$(PACKAGE).1.in >$@.tmp
	mv $@.tmp $@

clean-local:
	rm -f $(pkglibexec_SCRIPTS) $(PACKAGE)

rockspecs:
	rm -f *.rockspec
	$(LUA) mkrockspecs.lua $(PACKAGE) $(VERSION)
	$(LUA) mkrockspecs.lua $(PACKAGE) git

tag-release:
	git diff --exit-code && \
	git tag -a -m "Release tag" v$(VERSION) && \
	git push && git push --tags

check-in-release: distcheck
	cd ../$(PACKAGE)-release && \
	tar zxf ../$(PACKAGE)/$(PACKAGE)-$(VERSION).tar.gz && \
	cp -af $(PACKAGE)-$(VERSION)/* . && \
	rm -rf $(PACKAGE)-$(VERSION)/ && \
	git add . && git ci -m "Release v$(VERSION)" && \
	git tag -a -m "Full source release tag" release-v$(VERSION) && \
	git push && git push --tags

release: rockspecs
	$(MAKE) tag-release && \
	$(MAKE) check-in-release && \
	LUAROCKS_CONFIG=$(abs_srcdir)/luarocks-config.lua luarocks --tree=$(abs_srcdir)/luarocks build $(PACKAGE)-$(VERSION)-1.rockspec && \
	woger lua package=$(PACKAGE) package_name=$(PACKAGE_NAME) version=$(VERSION) description="`LUA_INIT= LUA_PATH='$(abs_srcdir)/?-git-1.rockspec' $(LUA) -l$(PACKAGE) -e 'print (description.summary)'`" notes=release-notes-$(VERSION) home="`LUA_INIT= LUA_PATH='$(abs_srcdir)/?-git-1.rockspec' $(LUA) -l$(PACKAGE) -e 'print (description.homepage)'`"
