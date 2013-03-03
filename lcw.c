/* cw (color wrapper) Lua extensions

   Copyright (c) 2013 Reuben Thomas <rrt@sc3d.org>
   Copyright (C) 2004 v9/fakehalo [v9@fakehalo.us]

   This file is part of cw.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lua.h>
#include <lauxlib.h>
#include "lua52compat.h"

static pid_t pid_c;
static int slave;

static int pusherror(lua_State *L, const char *info)
{
 lua_pushnil(L);
 lua_pushfstring(L, "%s: %s", info, strerror(errno));
 lua_pushinteger(L, errno);
 return 3;
}

static int _pid_c(lua_State *L){
 lua_pushinteger(L, pid_c);
 return 1;
}

/* Wrap a child process's I/O line by line.
   Returns nil from child process, and exit code from parent process, which is -1 if it was interrupted. */
static int wrap_child(lua_State *L){
 luaL_checktype(L,1,LUA_TFUNCTION);
 slave = luaL_checkint(L, 2);
 switch((pid_c=fork())){
 case -1:
  return pusherror(L,"fork() error.");
 case 0:
  /* child process to execute the program. */
  if(dup2(slave,STDOUT_FILENO)<0)
   return pusherror(L,"dup2() failed.");
#ifdef HAVE_SETSID
  setsid();
#endif
  lua_pushnil(L);
  break;
 default:
  /* parent process to filter the program's output; kills children if interrupted. */
  lua_pushvalue(L, 1);
  lua_pcall(L,0,0,0); /* Ignore errors. */
 }
 if(pid_c){
  int e=0;
  lua_pushinteger(L,waitpid(pid_c,&e,0)>=0&&WIFEXITED(e)?WEXITSTATUS(e):0);
 }
 return 1;
}

static int Gcanonicalize_file_name (lua_State *L)
{
 char *t = canonicalize_file_name (luaL_checkstring(L, 1));
 if (t) {
  lua_pushstring(L, t);
  free(t);
 } else
  lua_pushnil(L);
 return 1;
}

static const luaL_Reg R[] =
{
 {"canonicalize_file_name", Gcanonicalize_file_name},
 {"wrap_child",             wrap_child},
 {"pid_c",                  _pid_c},
 {NULL,	NULL}
};

LUALIB_API int luaopen_consolewrap(lua_State *L);

LUALIB_API int luaopen_consolewrap(lua_State *L)
{
 luaL_register(L, "consolewrap", R);
 return 1;
}
