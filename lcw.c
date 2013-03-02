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

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lua.h>
#include <lauxlib.h>
#include "lua52compat.h"
#include "unused-parameter.h"

static bool ext=false;
static pid_t pid_c;
static jmp_buf exitbuf;
static int master,slave;

static void int_handler(int sig _GL_UNUSED_PARAMETER){
 write(STDOUT_FILENO,"\x1b[00m",5);
 if(pid_c)
#if defined(HAVE_SETSID) && defined(HAVE_KILLPG)
  killpg(pid_c,SIGINT);
#else
 kill(pid_c,SIGTERM); /* SIGINT does not work; SIGTERM leaves any
                         sub-shells orphaned, but it's the best we can
                         do simply without process groups. */
#endif
 longjmp(exitbuf, 1);
}

static void chld_handler(int sig _GL_UNUSED_PARAMETER){
 fcntl(master,F_SETFL,O_NONBLOCK);
 ext=true;
}

static void sig_catch(int sig, int flags, void (*handler)(int), struct sigaction *oldact)
{
 struct sigaction sa;
 sa.sa_handler = handler;
 sa.sa_flags = flags;
 sigemptyset(&sa.sa_mask);
 assert(sigaction(sig, &sa, oldact)==0);
}

static int pusherror(lua_State *L, const char *info)
{
 lua_pushnil(L);
 lua_pushfstring(L, "%s: %s", info, strerror(errno));
 lua_pushinteger(L, errno);
 return 3;
}

/* Wrap a child process's I/O line by line.
   Returns nil from child process, and exit code from parent process, which is -1 if it was interrupted. */
static int wrap_child(lua_State *L){
 void *ud;
 lua_Alloc lalloc=lua_getallocf(L,&ud);
 luaL_checktype(L,1,LUA_TFUNCTION);
 master = luaL_checkint(L, 2);
 slave = luaL_checkint(L, 3);
 struct sigaction oldchldact,oldintact;
 sig_catch(SIGCHLD,SA_NOCLDSTOP,chld_handler,&oldchldact);
 sigaction(SIGINT,NULL,&oldintact);
 if(setjmp(exitbuf))
   lua_pushinteger(L,-1);
 else {
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
   {
    /* parent process to filter the program's output; kills children if interrupted. */
    sig_catch(SIGINT,0,int_handler,NULL);
    char *linebuf=NULL,*p=NULL;
    ssize_t size=0;
    char tmp[BUFSIZ];
    for(ssize_t s=0;(s=read(master,tmp,BUFSIZ))>0||!ext;){
     size_t off=p-linebuf;
     if((linebuf=lalloc(ud,linebuf,size,size+s))==NULL)
      return pusherror(L,"lalloc");
     p=linebuf+off;
     memcpy(linebuf+size,tmp,s);
     size+=s;
     char *q;
     while((q=memmem(p,size-(p-linebuf),"\r\n",2))){
      size_t len=q-p;
      lua_pushvalue(L,1);
      lua_pushlstring(L,p,len);
      lua_pcall(L,1,1,0); /* Ignore errors. */
      const char *text=lua_tolstring(L,-1,&len);
      if(text){
       write(STDOUT_FILENO,text,len);
       write(STDOUT_FILENO,"\n",1);
      }
      lua_pop(L,1);
      p=q+2;
     }
    }
    lalloc(L,linebuf,size,0);
   }
  }
 }
 if(pid_c){
  int e=0;
  lua_pushinteger(L,waitpid(pid_c,&e,0)>=0&&WIFEXITED(e)?WEXITSTATUS(e):0);
 }
 sigaction(SIGCHLD,&oldchldact,NULL);
 sigaction(SIGINT,&oldintact,NULL);
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
 {NULL,	NULL}
};

LUALIB_API int luaopen_consolewrap(lua_State *L);

LUALIB_API int luaopen_consolewrap(lua_State *L)
{
 luaL_register(L, "consolewrap", R);
 return 1;
}
