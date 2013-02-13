/* cw -- customizable color wrapper for common unix commands.
** Copyright (C) 2004 v9/fakehalo [v9@fakehalo.us]
** Copyright (c) 2013 Reuben Thomas <rrt@sc3d.org>
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
#include <stdio.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pty.h>
#include <ctype.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "progname.h"
#include "dirname.h"
#include "gl_xlist.h"
#include "gl_linked_list.h"
#include "hash.h"
#include "xalloc.h"
#include "minmax.h"

/* Match instruction. */
typedef struct{
  char *data;
  unsigned char col;
}match;

/* Colormap hash entry. */
typedef struct{
 const char *log;
 const char *phys;
}colormap_t;

char id[]="$Id: cw.c,v "VERSION" v9/fakehalo Exp $";

static bool ext=false;
static Hash_table *colormap;
static signed char base_color;
static pid_t pid_c;
static gl_list_t matches;

static const char **color_name;
static const char *color_name_real[]={"black","blue","green","cyan","red","purple","brown",
 "grey+","grey","blue+","green+","cyan+","red+","purple+","yellow","white",
 "default"};
static const char *color_name_real_invert[]={"white","blue+","green+","cyan+","red+","purple+",
 "yellow","grey","grey+","blue","green","cyan","red","purple","brown","black",
 "default"};
/* #define so that initializers are both constant without repeating the expression. */
#define COLORS sizeof(color_name_real)/sizeof(*color_name_real)
static size_t colors=COLORS;
static size_t default_color=COLORS-1;
static const char *color_code[]={"\x1b[00;30m","\x1b[00;34m","\x1b[00;32m",
 "\x1b[00;36m","\x1b[00;31m","\x1b[00;35m","\x1b[00;33m","\x1b[00;37m",
 "\x1b[01;30m","\x1b[01;34m","\x1b[01;32m","\x1b[01;36m","\x1b[01;31m",
 "\x1b[01;35m","\x1b[01;33m","\x1b[01;37m","\x1b[00m"};

/* Exit with or without a reason. */
noreturn void cwexit(signed char code,const char *reason){
 if(reason)fprintf(stdout,"cw:exit: %s\n",reason);
 fflush(stdout);
 fflush(stderr);
 exit(code);
}

void xalloc_die(void) {
 cwexit(1,"malloc() failed.");
}

/* Parse the n-th delim-delimited token out of a string. */
static char *parameter(const char *string,const char *delim,size_t n){
 char *fptr=xstrdup(string);
 char *arg=strtok(fptr,delim);
 while(n&&(arg=strtok(0,delim)))n--;
 arg=arg?xstrdup(arg):NULL;
 free(fptr);
 return arg;
}

/* Filter a directory out of a PATH-like colon-separated path list. */
static char *remove_dir_from_path(const char *path, const char *dir){
 if(path){
  char *canon_dir=canonicalize_file_name(dir);
  if(!canon_dir)return(xstrdup(path));
  size_t s=strlen(path),i=0;
  char *newpath=(char *)xzalloc(s+1);
  char *tmp=xstrdup(path),*ptr;
  while((ptr=parameter(tmp,":",i++))){
   char *canon_pptr=canonicalize_file_name(ptr);
   if(strcmp(canon_pptr,canon_dir)){
    if(*newpath)
     strcat(newpath,":");
    strcat(newpath,ptr);
   }
   free(canon_pptr);
   free(ptr);
  }
  free(tmp);
  free(canon_dir);
  return newpath;
 }
 return 0;
}

static void usage(void){
 fprintf(stdout,"Usage: not for direct use; use via definition files (see cw(1)):\n"
         "  --version             display version information and exit\n"
         "  --help                display this help and exit\n");
 cwexit(0,0);
}

/* Convert a logical color string to a physical color array index. (0..colors - 1).
   Returns -1 if color type undefined and no base color defined. */
static _GL_ATTRIBUTE_PURE signed char color_atoi(const char *color){
 colormap_t *c=XZALLOC(colormap_t);
 assert(color);
 c->log=color;
 colormap_t *ent=hash_lookup(colormap,c);
 if(!ent){ /* Use "base" if the color type is undefined. */
  c->log="base";
  ent=hash_lookup(colormap,c);
 }
 free(c);
 if(ent)
  for(signed char i=0;i<(signed)colors;i++)
   if(!strcmp(color_name[i],ent->phys))return(i);
 return(-1);
}

const char *default_colormap="base=cyan:bright=cyan+:highlight=green+:lowlight=green:neutral=white:warning=yellow:error=red+:punctuation=blue+";

static size_t colormap_hash (const void *c, size_t n){
 return hash_string(((const colormap_t *)c)->log,n);
}

static bool colormap_cmp (const void *c1, const void *c2){
 return !strcmp(((const colormap_t *)c1)->log, ((const colormap_t *)c2)->log);
}

/* Set user color map. */
static void setcolors(const char *str){
 char *ass,*tmp=xstrdup(str);
 for(size_t i=0;(ass=parameter(tmp,":",i));i++){
  char *tmp2=strtok(ass,"=");
  if(tmp2&&strlen(tmp2)){
   char *log=xstrdup(tmp2);
   char *phys=strtok(0,"");
   if(phys&&strlen(phys)){
    colormap_t *c=XZALLOC(colormap_t);
    c->log=xstrdup(log);
    c->phys=xstrdup(phys);
    hash_delete(colormap,c); /* Later entries override earlier ones. */
    assert(hash_insert(colormap,c));
   }
   else cwexit(1,"physical color missing or invalid.");
   free(log);
  }
  else cwexit(1,"logical color missing or invalid.");
  free(ass);
 }
 free(tmp);
 base_color=color_atoi("base");
 if(base_color==-1)
  setcolors("base=default");
}

/* Create a coloring array for a string. */
static unsigned char *make_colors(const char *string){
 size_t s=strlen(string);
 unsigned char *buf=xzalloc(s);
 memset(buf,base_color,s); /* Fill color array with base color. */
 const match *m;
 for(gl_list_iterator_t i=gl_list_iterator(matches);gl_list_iterator_next(&i,(const void **)&m,NULL);){
  regex_t re;
  if(!regcomp(&re,m->data,REG_EXTENDED)){
   regmatch_t pm;
   for(size_t j=0;j<s&&!regexec(&re,string+j,1,&pm,j?REG_NOTBOL:0);j+=pm.rm_eo){
    memset(buf+j+pm.rm_so,m->col,pm.rm_eo-pm.rm_so);
    if(pm.rm_eo==0)j++; /* Make sure we advance at least one character. */
   }
   regfree(&re);
  }
 }
 return(buf);
}

/* Color a string given a coloring array. */
static char *apply_colors(const char *string, const unsigned char *color){
 size_t j=0,s=strlen(string);
 char *tbuf=xzalloc((s+1)*(8+1)); /* longest escape sequence is 8 characters, +1 for the text. */
 unsigned char col=255; /* Invalid value to guarantee immediate change of color. */
 for(size_t i=0;i<s;i++){
  if(col!=color[i]){
   const char *esc=color_code[color[i]];
   col=color[i];
   strcpy(tbuf+j,esc);
   j+=strlen(esc);
  }
  tbuf[j++]=string[i];
 }
 if(col!=default_color)
  strcpy(tbuf+j,color_code[default_color]);
 return(tbuf);
}

/* Color a string based on the definition file. */
static char *convert_string(const char *string){
 unsigned char *color=make_colors(string);
 char *colored_line=apply_colors(string,color);
 free(color);
 return(colored_line);
}

static void sighandler(int sig){
 if(sig==SIGINT&&pid_c)
  kill(pid_c,SIGINT);
#ifdef SIGCHLD
 else if(sig==SIGCHLD)ext=true;
#endif
 if(sig==SIGPIPE||sig==SIGINT){
  fprintf(stderr,"%s",color_code[default_color]);
  cwexit(0,0);
 }
}

static void sig_catch(int sig, int flags, void (*handler)(int))
{
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = flags;
  sigemptyset(&sa.sa_mask);
  assert(sigaction(sig, &sa, 0)==0);
}

/* Set up coloring harness for a program. */
void set_up_harness(void){
 bool nocolor_stdout=!isatty(STDOUT_FILENO);
 bool nocolor_stderr=!isatty(STDERR_FILENO);
 int fds[2],fde[2];
 if(pipe(fds)<0)cwexit(1,"pipe() failed.");
 if(pipe(fde)<0)cwexit(1,"pipe() failed.");
 int master[2],slave[2];
 bool ptys_on=(openpty(&master[0],&slave[0],0,0,0)==0)&&
  (openpty(&master[1],&slave[1],0,0,0)==0);
#ifdef SIGCHLD
 sig_catch(SIGCHLD,SA_NOCLDSTOP,sighandler);
#endif
 sig_catch(SIGPIPE,0,sighandler);
 switch((pid_c=fork())){
  case -1:
   cwexit(1,"fork() error.");
   break;
  case 0:
   /* child process to execute the program. */
   if(dup2((ptys_on?slave[0]:fds[1]),STDOUT_FILENO)<0)
    cwexit(1,"dup2() failed.");
   close(fds[0]);
   close(fds[1]);
   if(dup2((ptys_on?slave[1]:fde[1]),STDERR_FILENO)<0)
    cwexit(1,"dup2() failed.");
   close(fde[0]);
   close(fde[1]);
#ifdef HAVE_SETSID
   setsid();
#endif
   return;
  default:
   /* parent process to filter the program's output. (forwards SIGINT to child) */
   sig_catch(SIGINT,0,sighandler);
   char buf[BUFSIZ+1];
   if(ptys_on){
    close(fds[0]);
    close(fde[0]);
    fds[0]=master[0];
    fde[0]=master[1];
   }
   fcntl(fds[0],F_SETFL,O_NONBLOCK);
   fcntl(fde[0],F_SETFL,O_NONBLOCK);
   int fdm=MAX(fds[0],fde[0])+1,fd=0,e=0;
   char *tmp=NULL;
   ssize_t j=0;
   for(ssize_t s=0;s>0||!ext;){
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fds[0],&rfds);
    FD_SET(fde[0],&rfds);
    if(select(fdm,&rfds,0,0,0)>=0){
     if(FD_ISSET(fds[0],&rfds))fd=fds[0];
     else if(FD_ISSET(fde[0],&rfds))fd=fde[0];
     else continue;
     while((s=read(fd,buf,BUFSIZ))>0){
      tmp=(char *)xrealloc(tmp,s+j+1);
      for(ssize_t i=0;s>i;i++){
       switch(buf[i]){
        case '\n':
         {
          char *aptr=NULL;
          tmp[j]=0;
          if(fd==fds[0])
           fprintf(stdout,"%s\n",nocolor_stdout?tmp:(aptr=convert_string(tmp)));
          else
           fprintf(stderr,"%s\n",nocolor_stderr?tmp:(aptr=convert_string(tmp)));
          free(aptr);
          fflush(fd==fds[0]?stdout:stderr);
          free(tmp);
          tmp=NULL;
          j=0;
          if(s>i)
           tmp=(char *)xzalloc(s-i+1);
          j=0;
         }
         break;
        case '\r':
         break;
        default:
         tmp[j++]=buf[i];
         break;
       }
      }
     }
    }
   }
   free(tmp);
   fflush(stdout);
   fflush(stderr);
   cwexit(waitpid(pid_c,&e,WNOHANG)>=0&&WIFEXITED(e)?WEXITSTATUS(e):0,0);
   break;
 }
}

static int add_match(lua_State *L) {
 match *m=(match *)XZALLOC(match);
 m->col=color_atoi(luaL_checkstring(L,1));
 m->data=xstrdup(luaL_checkstring(L,2));
 gl_list_add_last(matches,m);
 return 0;
}

static void getargs (lua_State *L,int argc,char **argv,int n) {
 lua_createtable(L,argc-(n+1),0);
 for (int i=n+1;i<argc;i++) {
  lua_pushstring(L,argv[i]);
  lua_pushvalue(L,-1);
  lua_rawseti(L,-3,i-n);
  lua_pushboolean(L,1);
  lua_rawset(L,-3);
 }
}

int main(int argc,char **argv,char **envp){
 set_program_name(argv[0]);
 if(argc<=1||!strcmp("--help",argv[1]))
  usage();
 else if(!strcmp("--version",argv[1]))
  cwexit(1,"cw (color wrapper) v"VERSION);
 char *scrname=xstrdup(argv[1]);
 char *base_scrname=base_name(scrname?scrname:program_name);
 matches=gl_list_create_empty(GL_LINKED_LIST,NULL,NULL,NULL,1);
 colormap=hash_initialize(16,NULL,colormap_hash,colormap_cmp,NULL);
 color_name=getenv("CW_INVERT")?color_name_real_invert:color_name_real;
 char *ptr=getenv("CW_COLORS");
 setcolors(ptr?ptr:default_colormap);
 char *newpath=remove_dir_from_path(getenv("PATH"),SCRIPTSDIR);
 setenv("PATH",newpath,1);
 free(newpath);
 lua_State *L=luaL_newstate();
 assert(L);
 luaL_openlibs(L);
 lua_register(L,"match",add_match);
 getargs(L,argc,argv,1);
 lua_setglobal(L,"arg");
 if(luaL_dofile(L,scrname))
  cwexit(1,"definition file cannot be read or contains an error.");
 const char *cmd=base_scrname;
 lua_getglobal(L,"command");
 if(lua_isstring(L,-1)){
  char *cmdline=xstrdup(lua_tostring(L,-1));
  if(!cmdline)
   cwexit(1,"invalid command given.");
  cmd="/bin/sh";
  argv=xzalloc(sizeof(char*)*(3+1)); /* argv needs a trailing NULL. */
  argv[0]=base_scrname;
  argv[1]=xstrdup("-c");
  argv[2]=cmdline;
 }
 else argv++;
 if(!getenv("NOCOLOR"))
  set_up_harness();
 execvpe(cmd,argv,envp);
}
