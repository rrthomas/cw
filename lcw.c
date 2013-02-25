/* console wrapper: Lua library to wrap the console output of a child process
** Copyright (c) 2013 Reuben Thomas <rrt@sc3d.org>
**
** Based on cw (color wrapper)
** Copyright (C) 2004 v9/fakehalo [v9@fakehalo.us]
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**/

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
#include <pty.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lua.h>
#include <lauxlib.h>
#include "lua52compat.h"

static bool ext=false;
static pid_t pid_c;
static jmp_buf exitbuf;
static int master,slave;

static void sighandler(int sig){
 if(sig==SIGINT&&pid_c)
  kill(pid_c,SIGINT);
 else if(sig==SIGCHLD){
  fcntl(master,F_SETFL,O_NONBLOCK);
  ext=true;
 }
 if(sig==SIGINT){
  write(STDOUT_FILENO,"\x1b[00mSIGINT",11);
  if(pid_c)
   exit(0);
  else
   longjmp(exitbuf, 1);
 }
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
 if (info==NULL)
  lua_pushstring(L, strerror(errno));
 else
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
 if(openpty(&master,&slave,0,0,0))
  return pusherror(L,"openpty error.");
 struct sigaction oldchldact,oldintact;
 sig_catch(SIGCHLD,SA_NOCLDSTOP,sighandler,&oldchldact);
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
    /* parent process to filter the program's output. (forwards SIGINT to child) */
    sig_catch(SIGINT,0,sighandler,NULL);
    char *linebuf=NULL,*p=NULL;
    ssize_t size=0;
    for(ssize_t s=0;s>0||!ext;){
     char tmp[BUFSIZ];
     while((s=read(master,tmp,BUFSIZ))>0){
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
       if(p-linebuf>=size){
        /* Whenever we completely empty the buffer, free it, to try to avoid
           using too much memory. */
        lalloc(L,linebuf,size,0);
        p=linebuf=NULL;
        size=0;
        break;
       }
      }
     }
    }
    lalloc(L,linebuf,size,0);
    int e=0;
    lua_pushinteger(L,waitpid(pid_c,&e,WNOHANG)>=0&&WIFEXITED(e)?WEXITSTATUS(e):0);
   }
  }
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
