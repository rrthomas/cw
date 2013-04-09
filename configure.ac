dnl configure.ac
dnl
dnl Copyright (c) 2013 Reuben Thomas <rrt@sc3d.org>
dnl
dnl This file is part of cw.
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published
dnl by the Free Software Foundation; either version 3, or (at your
dnl option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

dnl Initialise autoconf and automake
AC_INIT([cw], [2.0.1], [rrt@sc3d.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl Checks for programs
AX_PROG_LUA

dnl Generate output
AC_CONFIG_FILES([Makefile luarocks-config.lua cw.lua cw-definitions-path.lua])
AC_OUTPUT
