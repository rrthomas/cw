/* cw -- customizable color wrapper for common unix commands.
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

#include <stdio.h>
#include <stdnoreturn.h>
#ifndef __USE_BSD
#define __USE_BSD
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif
#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif
#ifdef __CYGWIN__
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif
#endif
#include <stdlib.h>
#ifndef __USE_POSIX
#define __USE_POSIX
#endif
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UTIL_H
#include <util.h>
#elif HAVE_LIBUTIL_H
#include <libutil.h>
#endif
#ifdef HAVE_PTY_H
#include <pty.h>
#endif
#include <ctype.h>
#include <regex.h>
#ifndef REG_EXTENDED
#define REG_EXTENDED 1
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_UNIX_H
#include <unix.h>
#endif
#include "cw.h"

#ifndef HAVE_GETPT
#ifndef HAVE_OPENPTY
#ifndef HAVE_LIBUTIL
#ifndef HAVE_DEV_PTMX
#define NO_PTY
#endif
#endif
#endif
#endif
#ifndef HAVE_SETENV
#ifndef HAVE_PUTENV
#define NO_ENVSET
#endif
#endif
#define INT_SETPROCTITLE
#ifndef __APPLE_CC__
#ifndef __linux__
#undef INT_SETPROCTITLE
#endif
#endif
#ifndef INT_SETPROCTITLE
#ifndef HAVE_SETPROCTITLE
#define NO_SETPROCTITLE
#endif
#endif
#ifndef I_PUSH
#define I_PUSH 21250
#endif

/* prototypes. */
void sighandler(signed int);
char *strpname(char *);
unsigned char strwcmp(char *,char *);
#ifdef HAVE_UNAME
unsigned char struncmp(char *);
#endif
unsigned char regxcmp(char *,char *,unsigned char);
signed char color_atoi(char *);
signed char make_ptypair(unsigned char v);
unsigned char cwprintf(char *);
void setcolorize(char *);
signed char execot(char *,unsigned char,unsigned int);
void execcw(signed int,char **,signed int,char **);
#ifndef NO_SETPROCTITLE
void initsetproctitle(signed int,char **,char **);
#ifndef HAVE_SETPROCTITLE
void setproctitle(const char *,...);
#endif
#endif
void c_handler(char *,unsigned int,signed int);
void c_read(char *,signed int);
void c_error(unsigned int,const char *);
void cwexit(signed char,const char *);

/* pseudo-setproctitle table. */
#ifndef HAVE_SETPROCTITLE
#ifdef INT_SETPROCTITLE
struct{
 char **argv;
 char *largv;
 char *name;
}proct;
#endif
#endif
/* configuration table. */
struct{
 unsigned char col;
 signed char addhelp;
 signed char ifarg;
 signed char ifarga;
 signed char ifexit;
 signed char ifexita;
 signed char iflabel;
 signed char iflabelf;
 signed char ifos;
 signed char ifosa;
 signed char base;
 signed char clear;
 signed char ec;
 signed char eint;
 signed char fc;
 signed char ifregex;
 signed char ign;
 signed char invert;
 signed char nocolor;
 signed char noeol;
 signed char noer;
 signed char nopipe;
 signed char nostrip;
 signed char po;
 signed char pty;
 signed char ron;
 char *path;
 char *cmd;
 char *cmdargs;
 char *label;
#ifndef NO_SETPROCTITLE
 char *title;
#endif
 struct{
  char **data;
  unsigned char *b;
  unsigned char *a;
  unsigned int cur;
  unsigned int tot;
 }m;
 struct{
  char **data;
  unsigned char *b;
  unsigned char *a;
  unsigned int cur;
  unsigned int tot;
 }x;
 struct{
  unsigned char *slot;
  unsigned char *delim;
  unsigned char *b;
  unsigned char *a;
  unsigned int cur;
  unsigned int tot;
 }t;
 struct{
  unsigned char b;
  unsigned char a;
  unsigned char on;
 }n;
 struct{
  unsigned char b;
  unsigned char a;
  unsigned char on;
 }u;
 struct{
  unsigned char b;
  unsigned char a;
  unsigned char on;
 }l;
 struct{
  signed char l;
  signed char h;
  signed char c;
 }r;
 struct{
  signed char l;
  signed char h;
  signed char on;
 }z;
 struct{
  char *ldata;
  char *rdata;
  unsigned int llen;
  unsigned int rlen;
 }b;
#ifndef NO_PTY
 struct{
  signed int mout;
  signed int sout;
  signed int merr;
  signed int serr;
  signed char on;
 }p;
#endif
 struct itimerval w;
}cfgtable;

char id[]="$Id: cw.c,v "VERSION" v9/fakehalo Exp $";

unsigned char ext=0,rexit=0;
char *pal2[18],*aptr,*fptr,*pptr,*progname,*scrname;
pid_t pid_p,pid_c;
extern char **environ;

static const char *pal1[]={"black","blue","green","cyan","red","purple","brown",
 "grey+","grey","blue+","green+","cyan+","red+","purple+","yellow","white",
 "default","none",""};
static const char *pal1_invert[]={"white","blue+","green+","cyan+","red+","purple+",
 "yellow","grey","grey+","blue","green","cyan","red","purple","brown","black",
 "default","none",""};
static const char *pal2_orig[]={"\x1b[00;30m","\x1b[00;34m","\x1b[00;32m",
 "\x1b[00;36m","\x1b[00;31m","\x1b[00;35m","\x1b[00;33m","\x1b[00;37m",
 "\x1b[01;30m","\x1b[01;34m","\x1b[01;32m","\x1b[01;36m","\x1b[01;31m",
 "\x1b[01;35m","\x1b[01;33m","\x1b[01;37m","\x1b[0m",""};
static const char *cfgmsg[]={
 "invalid definition instruction.",
 "no valid 'path' or 'other' definition was defined.",
 "'match' definition used an invalid color. (defaulting)",
 "'match' too many entries. (race condition?)",
 "'match' syntax error. (not enough arguments?)",
 "'token' definition used an invalid color. (defaulting)",
 "'token' slot value out of range. (0-255)",
 "'token' delim value out of range. (1-255)",
 "'token' too many entries. (race condition?)",
 "'token' syntax error. (not enough arguments?)",
 "'path' definition contained no existing/usable paths.",
 "'base' definition used an invalid color.",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "'digit' definition used an invalid color. (defaulting)",
 "'wait' definition used an invalid time value.",
 "'ucase' definition used an invalid color. (defaulting)",
 "'lcase' definition used an invalid color. (defaulting)",
 "'ifarg'/'ifnarg' syntax error. (not enough arguments?)",
 "'limit' syntax error. (not enough arguments?)",
 "'other' syntax error. (not enough arguments?)",
 "environment variable placement syntax error. (not enough arguments?)",
 "both 'path' and 'other' were defined. (one definition or the other)",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "'regex' definition used an invalid color. (defaulting)",
 "'regex' too many entries. (race condition?)",
 "'regex' syntax error. (not enough arguments?)",
 "'ifos'/'ifnos' syntax error. (not enough arguments?)",
 "'ifos-else' used before any previous comparison.",
 "'ifarg-else' used before any previous comparison.",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "'!'/'@' failed to execute background program.",
 "'ifexit'/'ifnexit' invalid exit level. (-127..127)",
 "'ifexit-else' used before any previous comparison.",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "NO SUCH ERROR",
 "'print' syntax error. (not enough arguments?)",
 "'print' write error.",
 "NO SUCH ERROR",
 "configuration contained critical errors."};

/* program start. */
signed int main(signed int argc,char **argv){
 int i=0,j=0, margc=0;
 char *ptr,**margv;
 cfgtable.z.l=cfgtable.z.h=-1;
 if(!(margv=(char **)malloc((sizeof(char *)*(argc+1)))))
  cwexit(1,"malloc() failed.");
 margv[0]=argv[0];
 for(margc=i=1;i<argc;i++){
  if(!strcmp("+nc",argv[i])||!strcmp("--cw-nocolor",argv[i]))
   cfgtable.nocolor=1;
  else if(!strcmp("+iv",argv[i])||!strcmp("--cw-invert",argv[i]))
   cfgtable.invert=1;
#ifndef NO_PTY
  else if(!strcmp("+py",argv[i])||!strcmp("--cw-usepty",argv[i]))
   cfgtable.pty=1;
#endif
  else if(!strncmp("+co=",argv[i],4)){
   if(strlen(argv[i])>4)setcolorize(argv[i]+4);
  }
  else if(!strncmp("--cw-colorize=",argv[i],14)){
   if(strlen(argv[i])>14)setcolorize(argv[i]+14);
  }
  else if(!strcmp("--help",argv[i])){
   cfgtable.addhelp=1;
  }
  else if(!strcmp("-v",argv[i])){
   cwexit(1,"cw (color wrapper) v"VERSION" (support=+"
#ifndef NO_ENVSET
    "e"
#endif
#ifdef HAVE_UNAME
    "o"
#endif
#ifndef NO_PTY
    "p"
#endif
    "r"
#ifndef NO_SETPROCTITLE
    "s"
#endif
    "t"
#ifdef HAVE_WAITPID
    "w"
#endif
    ")");
   }
  else margv[margc++]=argv[i];
 }
 margv[margc]=0;
 if(!(progname=(char *)malloc(strlen(margv[0])+1)))
  cwexit(1,"malloc() failed.");
 strcpy(progname,margv[0]);
 cfgtable.po=0;
 if(strlen(progname)>2&&!strncmp(strpname(progname),"cwe",3))
  cfgtable.po=1;
 for(i=(cfgtable.po?1:2);margc>i;i++)j+=(strlen(margv[i])+1);
 if(!(cfgtable.cmdargs=(char *)malloc(j+1)))
  cwexit(1,"malloc() failed.");
 memset(cfgtable.cmdargs,0,(j+1));
 j=0;
 for(i=(cfgtable.po?1:2);margc>i;i++){
  sprintf(cfgtable.cmdargs+j,"%s%c",margv[i],(margc-i==1?0:32));
  j+=(strlen(margv[i])+1);
 }
 if(!cfgtable.po){
  if(margc>1){
   if(access(margv[1],F_OK))
    cwexit(1,"non-existent path to definition file.");
   if(!(scrname=(char *)malloc(strlen(margv[1])+1)))
    cwexit(1,"malloc() failed.");
   strcpy(scrname,margv[1]);
  }
  else if (argc == 1)
   cwexit(1,"this program is not intended to be used interactively.");
  else
   cwexit(0,0);
 }
 else if(cfgtable.po&&margc<2)
  cwexit(1,"no argument(s) supplied.");
 for(i=0;18>i;i++){
  if(!(pal2[i]=(char *)malloc(strlen(pal2_orig[i])+1)))
   cwexit(1,"malloc() failed.");
  memset(pal2[i],0,(strlen(pal2_orig[i])+1));
  strcpy(pal2[i],pal2_orig[i]);
 }
 if(!cfgtable.po){
#ifdef HAVE_WAITPID
  if(getenv("CW_CHK_SETCODE"))
   cfgtable.ec=execot(getenv("CW_CHK_SETCODE"),2,0);
#endif
  cfgtable.base=-1;
  cfgtable.col=cfgtable.ifarg=cfgtable.ifarga=0;
  cfgtable.ifos=cfgtable.ifosa=cfgtable.ifexit=cfgtable.ifexita=0;
  cfgtable.ron=cfgtable.iflabel=cfgtable.m.cur=cfgtable.t.cur=0;
  cfgtable.w.it_interval.tv_sec=cfgtable.w.it_value.tv_sec=0;
  cfgtable.w.it_interval.tv_usec=cfgtable.w.it_value.tv_usec=0;
#ifndef NO_PTY
  cfgtable.p.on=0;
  if(getenv("CW_USEPTY"))cfgtable.pty=1;
#endif
  if(getenv("CW_CLEAR"))cfgtable.clear=1;
 }
 if(!cfgtable.z.on&&(ptr=(char *)getenv("CW_COLORIZE")))
  setcolorize(ptr);
 if(getenv("CW_INVERT"))cfgtable.invert=1;
 if(cfgtable.po){
  if(cwprintf(cfgtable.cmdargs))
   cwexit(1,"write error.");
  cwexit(0,0);
 }
 c_read(scrname,margc);
 cfgtable.nocolor+=(getenv("NOCOLOR")?1:0);
 cfgtable.nocolor+=(getenv("MAKELEVEL")?1:0);
 if(getenv("CW_SHLVL")&&getenv("SHLVL")&&
 strcmp(getenv("CW_SHLVL"),getenv("SHLVL")))
  cfgtable.nocolor=1;
#ifdef HAVE_WAITPID
 if(!cfgtable.nocolor&&getenv("CW_CHK_NOCOLOR"))
  cfgtable.nocolor=(execot(getenv("CW_CHK_NOCOLOR"),2,0)?1:0);
#endif
 if(getenv("NOCOLOR_NEXT")){
#ifdef HAVE_SETENV
  setenv("NOCOLOR","1",1);
#elif HAVE_PUTENV
  putenv("NOCOLOR=1");
#endif
#ifdef HAVE_UNSETENV
  unsetenv("NOCOLOR_NEXT");
#endif
 }
 if(getenv("NOCOLOR_PIPE"))cfgtable.nopipe=1;
 /* from patch submitted by <komar@ukr.net>. (modified from original) */
 if(!isatty(STDOUT_FILENO)||!isatty(STDERR_FILENO)){
  if(cfgtable.nopipe){
   cfgtable.nocolor=1;
   cfgtable.clear=0;
  }
#ifndef NO_PTY
  else if(cfgtable.pty)cfgtable.pty=0;
#endif
 }
 if(cfgtable.z.on)cfgtable.invert=0;
 if(cfgtable.fc&&cfgtable.nocolor)cfgtable.nocolor=0;
 if(!cfgtable.nocolor&&cfgtable.clear){
  fprintf(stdout,"\x1b[H\x1b[2J");
  fflush(stdout);
 }
 execcw(argc,argv,margc,margv);
 cwexit(0,0);
 /* won't make it here. */
 exit(0);
}
/* all-purpose signal handler. */
void sighandler(signed int sig){
 if(sig==SIGINT&&cfgtable.eint){
  if(pid_c){
   kill(pid_c,SIGINT);
   cfgtable.eint=0;
  }
 }
#ifdef SIGCHLD
 else if(sig==SIGCHLD){
  if(pid_p)kill(pid_p,SIGUSR1);
 }
#endif
 else if(sig==SIGUSR1)ext=1;
 else if(sig==SIGALRM)ext=2;
 else if(sig==SIGPIPE||sig==SIGINT){
  fprintf(stderr,"%s",pal2[16]);
  fflush(stderr);
  cwexit(0,0);
 }
 return;
}
/* plucks a requested token out of a string. */
static char *parameter(const char *string,const char *delim,unsigned int p){
 unsigned int n=p;
 char *arg;
 free(fptr);
 if(!(fptr=arg=(char *)malloc(strlen(string)+1)))
  cwexit(1,"malloc() failed.");
 strcpy(arg,string);
 arg=strtok(arg,delim);
 while(n&&(arg=strtok(0,delim)))n--;
 if(!arg){
  free(fptr);
  if(!(fptr=arg=(char *)malloc(3)))cwexit(1,"malloc() failed.");
  strcpy(arg,"-1");
 }
 return(pptr=arg);
}
/* converts the original string to a color string based on the config file. */
static char *convert_string(const char *line){
 unsigned char on=0;
 unsigned int i=0,j=0,k=0,l=0,s=0;
 char *buf,*tbuf,*tmp;
 char *tmpcmp;
 regex_t re;
 regmatch_t pm;
 s=strlen(line);
 if(!(tbuf=(char *)malloc(s+1)))
  cwexit(1,"malloc() failed.");
 strcpy(tbuf,line);
 /* start processing the 'digit' definition. */
 if(cfgtable.n.on){
  if(!(tmp=(char *)malloc(s*16+1)))
   cwexit(1,"malloc() failed.");
  memset(tmp,0,(s*16+1));
  for(k=j=i=0;s>i;i++){
   if(isdigit((unsigned char)tbuf[i])){
    if(!on){
     if(cfgtable.n.b!=17){
      strcpy(tmp+k,pal2[cfgtable.n.b==16?cfgtable.base:cfgtable.n.b]);
      k+=strlen(pal2[cfgtable.n.b==16?cfgtable.base:cfgtable.n.b]);
     }
     on=1;
    }
   }
   else if(on){
    if(cfgtable.n.a!=17){
     strcpy(tmp+k,pal2[cfgtable.n.a==16?cfgtable.base:cfgtable.n.a]);
     k+=strlen(pal2[cfgtable.n.a==16?cfgtable.base:cfgtable.n.a]);
    }
    on=0;
   }
   tmp[k++]=tbuf[i];
  }
  free(tbuf);
  s=strlen(tmp);
  if(!(tbuf=(char *)malloc(s+1)))
   cwexit(1,"malloc() failed.");
  strcpy(tbuf,tmp);
  free(tmp);
  on=0;
 }
 /* start processing the 'ucase' definition. */
 if(cfgtable.u.on){
  if(!(tmp=(char *)malloc(s*16+1)))cwexit(1,"malloc() failed.");
  memset(tmp,0,(s*16+1));
  for(k=j=i=0;s>i;i++){
   if((!memchr(tmp+(k-(k<7?k:7)),'\x1b',(k<7?k:7)))
   &&isupper((unsigned char)tbuf[i])){
    if(!on){
     if(cfgtable.u.b!=17){
      strcpy(tmp+k,pal2[cfgtable.u.b==16?cfgtable.base:cfgtable.u.b]);
      k+=strlen(pal2[cfgtable.u.b==16?cfgtable.base:cfgtable.u.b]);
     }
     on=1;
    }
   }
   else if(on){
    if(cfgtable.u.a!=17){
     strcpy(tmp+k,pal2[cfgtable.u.a==16?cfgtable.base:cfgtable.u.a]);
     k+=strlen(pal2[cfgtable.u.a==16?cfgtable.base:cfgtable.u.a]);
    }
    on=0;
   }
   tmp[k++]=tbuf[i];
  }
  free(tbuf);
  s=strlen(tmp);
  if(!(tbuf=(char *)malloc(s+1)))
   cwexit(1,"malloc() failed.");
  strcpy(tbuf,tmp);
  free(tmp);
  on=0;
 }
 /* start processing the 'lcase' definition. */
 if(cfgtable.l.on){
  if(!(tmp=(char *)malloc(s*16+1)))
   cwexit(1,"malloc() failed.");
  memset(tmp,0,(s*16+1));
  for(k=j=i=0;s>i;i++){
   if((!memchr(tmp+(k-(k<7?k:7)),'\x1b',(k<7?k:7)))
   &&islower((unsigned char)tbuf[i])){
    if(!on){
     if(cfgtable.l.b!=17){
      strcpy(tmp+k,pal2[cfgtable.l.b==16?cfgtable.base:cfgtable.l.b]);
      k+=strlen(pal2[cfgtable.l.b==16?cfgtable.base:cfgtable.l.b]);
     }
     on=1;
    }
   }
   else if(on){
    if(cfgtable.l.a!=17){
     strcpy(tmp+k,pal2[cfgtable.l.a==16?cfgtable.base:cfgtable.l.a]);
     k+=strlen(pal2[cfgtable.l.a==16?cfgtable.base:cfgtable.l.a]);
    }
    on=0;
   }
   tmp[k++]=tbuf[i];
  }
  free(tbuf);
  s=strlen(tmp);
  if(!(tbuf=(char *)malloc(s+1)))cwexit(1,"malloc() failed.");
  strcpy(tbuf,tmp);
  free(tmp);
  on=0;
 }
 /* start processing the 'token' definitions. */
 for(i=0;i<cfgtable.t.tot;i++){
  s=strlen(tbuf);
  if(!(tmp=(char *)malloc(s+strlen(tbuf)+16+1)))
   cwexit(1,"malloc() failed.");
  memset(tmp,0,(s+strlen(tbuf)+16+1));
  if(!cfgtable.t.slot[i])on=3;
  else on=0;
  for(l=k=j=0;j<s;j++){
   if(tbuf[j]==cfgtable.t.delim[i]){
    if(!on){
     k++;
     if(k==cfgtable.t.slot[i])on=1;
     else on=4;
    }
    else if(on==2){
     if(cfgtable.t.a[i]!=17){
      strcpy(tmp+l,pal2[cfgtable.t.a[i]==16?cfgtable.base:cfgtable.t.a[i]]);
      l+=strlen(pal2[cfgtable.t.a[i]==16?cfgtable.base:cfgtable.t.a[i]]);
     }
     on=0;
    }
   }
   else if(on==1||on==3){
    if(cfgtable.t.b[i]!=17){
     strcpy(tmp+l,pal2[cfgtable.t.b[i]==16?cfgtable.base:cfgtable.t.b[i]]);
     l+=strlen(pal2[cfgtable.t.b[i]==16?cfgtable.base:cfgtable.t.b[i]]);
    }
    on=2;
   }
   else if(on==4)on=0;
   tmp[l++]=tbuf[j];
  }
  if(on==2){
   if(cfgtable.t.a[i]!=17){
    strcpy(tmp+l,pal2[cfgtable.t.a[i]==16?cfgtable.base:cfgtable.t.a[i]]);
    l+=strlen(pal2[cfgtable.t.a[i]==16?cfgtable.base:cfgtable.t.a[i]]);
   }
   on=0;
  }
  free(tbuf);
  if(!(tbuf=(char *)malloc(strlen(tmp)+1)))
   cwexit(1,"malloc() failed.");
  strcpy(tbuf,tmp);
  free(tmp);
 }
 /* start processing the 'match' definitions. */
 if(cfgtable.m.tot){
  s=strlen(tbuf);
  if(!(tmp=(char *)malloc(s*(cfgtable.m.tot*16+1)+strlen(tbuf)+1)))
   cwexit(1,"malloc() failed.");
  memset(tmp,0,(s*(cfgtable.m.tot*16+1)+strlen(tbuf)+1));
  for(k=i=0;i<s;i++){
   for(j=0;j<cfgtable.m.tot;j++){
    if((!memchr(tmp+(k-(k<7?k:7)),'\x1b',(k<7?k:7)))
    &&!strwcmp(tbuf+i,cfgtable.m.data[j])){
     if(cfgtable.m.b[j]!=17){
      strcpy(tmp+k,pal2[cfgtable.m.b[j]==16?cfgtable.base:cfgtable.m.b[j]]);
      k+=strlen(pal2[cfgtable.m.b[j]==16?cfgtable.base:cfgtable.m.b[j]]);
     }
     strncpy(tmp+k,tbuf+i,strlen(cfgtable.m.data[j]));
     k+=strlen(cfgtable.m.data[j]);
     i+=strlen(cfgtable.m.data[j]);
     if(cfgtable.m.a[j]!=17){
      strcpy(tmp+k,pal2[cfgtable.m.a[j]==16?cfgtable.base:cfgtable.m.a[j]]);
      k+=strlen(pal2[cfgtable.m.a[j]==16?cfgtable.base:cfgtable.m.a[j]]);
     }
     j=-1;
    }
   }
   tmp[k++]=tbuf[i];
  }
  free(tbuf);
  if(!(tbuf=(char *)malloc(strlen(tmp)+1)))
   cwexit(1,"malloc() failed.");
  strcpy(tbuf,tmp);
  free(tmp);
 }
 /* start processing the 'regex' definitions. */
 if(cfgtable.x.tot){
  for(j=i=0;i<cfgtable.x.tot;i++){
   s=strlen(tbuf);
   if(!(tmp=(char *)malloc(s*(cfgtable.x.tot*16+1)+s+1)))
    cwexit(1,"malloc() failed.");
   memset(tmp,0,(s*(cfgtable.x.tot*16+1)+s+1));
   on=j=l=k=0;
   if(regcomp(&re,cfgtable.x.data[i],(cfgtable.noer?0:REG_EXTENDED)))
    free(tmp);
   else{
    while(k<s&&!regexec(&re,tbuf+k,1,&pm,(k?REG_NOTBOL:0))){
     if(pm.rm_so){
      if(!(tmpcmp=(char *)malloc(pm.rm_so+1)))
       cwexit(1,"malloc() failed.");
      memset(tmpcmp,0,pm.rm_so+1);
      strncpy(tmpcmp,tbuf+k,pm.rm_so);
      strcpy(tmp+j,tmpcmp);
      j+=strlen(tmpcmp);
      free(tmpcmp);
     }
     if(!pm.rm_so&&!pm.rm_eo){
      pm.rm_eo++;
      on=1;
     }
     l=(pm.rm_eo-pm.rm_so);
     if(!(tmpcmp=(char *)malloc(l+1)))
      cwexit(1,"malloc() failed.");
     memset(tmpcmp,0,l+1);
     k+=pm.rm_so;
     strncpy(tmpcmp,tbuf+k,l);
     if(!on&&!memchr(tbuf+(k-(k<7?k:7)),'\x1b',(k<7?k:7))
     &&cfgtable.x.b[i]!=17){
      strcpy(tmp+j,pal2[cfgtable.x.b[i]==16?cfgtable.base:cfgtable.x.b[i]]);
      j+=strlen(pal2[cfgtable.x.b[i]==16?cfgtable.base:cfgtable.x.b[i]]);
     }
     strcpy(tmp+j,tmpcmp);
     j+=strlen(tmpcmp);
     free(tmpcmp);
     if(!on&&!memchr(tbuf+(k-(k<7?k:7)),'\x1b',(k<7?k:7))
     &&cfgtable.x.a[i]!=17){
      strcpy(tmp+j,pal2[cfgtable.x.a[i]==16?cfgtable.base:cfgtable.x.a[i]]);
      j+=strlen(pal2[cfgtable.x.a[i]==16?cfgtable.base:cfgtable.x.a[i]]);
     }
     on=0;
     k-=pm.rm_so;
     k+=pm.rm_eo;
    }
    regfree(&re);
    if(s>k)strcpy(tmp+j,tbuf+k);
    free(tbuf);
    if(!(tbuf=(char *)malloc(strlen(tmp)+1)))
     cwexit(1,"malloc() failed.");
    strcpy(tbuf,tmp);
    free(tmp);
    on=0;
   }
  }
 }
 if(!(buf=(char *)malloc(strlen(pal2[cfgtable.base])+strlen(tbuf)
 +cfgtable.b.llen+cfgtable.b.rlen+(!cfgtable.noeol?4:0)+1)))
  cwexit(1,"malloc() failed.");
 sprintf(buf,"%s%s%s%s%s",(cfgtable.b.llen?cfgtable.b.ldata:"")
 ,pal2[cfgtable.base],tbuf,(cfgtable.b.rlen?cfgtable.b.rdata:""),
 (!cfgtable.noeol?pal2[16]:""));
 free(tbuf);
 free(aptr);
 return(aptr=buf);
}
/* just like basename(), except no conflicts on different systems. */
_GL_ATTRIBUTE_PURE char *strpname(char *file){
 char *ptr=strrchr(file,'/');
 return(ptr?ptr+1:file);
}
/* scans for the match/wildcard in a string. */
_GL_ATTRIBUTE_PURE unsigned char strwcmp(char *line1,char *line2){
 unsigned int i=0,s=0,t=0;
 s=strlen(line1);
 t=strlen(line2);
 for(i=0;i<s&&i<t;i++)
  /* don't count wildcard matches of 0x1b. */
  if(line1[i]!=line2[i]&&!(line1[i]!='\x1b'&&line2[i]==-1))return(1);
 return(i==t?0:1);
}
#ifdef HAVE_UNAME
/* scans for a system name match. */
unsigned char struncmp(char *cmp){
 struct utsname un;
 if(uname(&un)<0||!strlen(un.sysname))return(1);
 if(!regxcmp(un.sysname,cmp,0))return(0);
 return(1);
}
#endif
/* checks for a regex match of a string, or strcmp-like if not supported. */
unsigned char regxcmp(char *str,char *pattern,unsigned char type){
 signed int r=0;
 regex_t re;
 if(cfgtable.ifregex){
  if(regcomp(&re,pattern,(cfgtable.noer?0:REG_EXTENDED)|REG_NOSUB))
   return(1);
  r=regexec(&re,str,0,0,0);
  regfree(&re);
  if(r)return(1);
  return(0);
 }
 else{
  if(!type){
   if(!strcasecmp(str,pattern))return(0);
  }
  else{
   if(strstr(str,pattern))return(0);
  }
 }
 return(1);
}
/* converts the color string to a numerical storage value. (0-17) */
_GL_ATTRIBUTE_PURE signed char color_atoi(char *color){
 unsigned char i=0;
 const char **palptr;
 if(cfgtable.invert)palptr=pal1_invert;
 else palptr=pal1;
 if(cfgtable.z.on){
  if(!strcmp(color,"default")){
   if(cfgtable.base<9)i=cfgtable.z.l;
   else i=cfgtable.z.h;
  }
  else if(!strcmp(color,"none"))i=17;
  else{
   if(!strchr(color,'+'))i=cfgtable.z.l;
   else i=cfgtable.z.h;
  }
 }
 else{
  while(strcmp(palptr[i],color)&&i<18)i++;
 }
 return(i<18?i:-1);
}
#ifndef NO_PTY
/* creates a pty pair. (master/slave) */
signed char make_ptypair(unsigned char v){
#ifdef HAVE_GETPT
 signed int master,slave;
 char *name;
 if((master=getpt())<0)return(0);
 if(grantpt(master)<0||unlockpt(master)<0){
  close(master);
  return(0);
 }
 if(!(name=ptsname(master))){
  close(master);
  return(0);
 }
 if((slave=open(name,O_RDWR))==-1){
  close(master);
  return(0);
 }
 if(isastream(slave)){
  if(ioctl(slave,I_PUSH,"ptem")<0||ioctl(slave,I_PUSH,"ldterm")<0){
   close(master);
   close(slave);
   return(0);
  }
 }
 if(v){
  cfgtable.p.merr=master;
  cfgtable.p.serr=slave;
 }
 else{
  cfgtable.p.mout=master;
  cfgtable.p.sout=slave;
 }
#elif HAVE_OPENPTY
 if(v){
  if(openpty(&cfgtable.p.merr,&cfgtable.p.serr,0,0,0))return(0);
 }
 else{
  if(openpty(&cfgtable.p.mout,&cfgtable.p.sout,0,0,0))return(0);
 }
#elif HAVE_DEV_PTMX
 signed int master,slave;
 char *name;
 if((master=open("/dev/ptmx",O_RDWR))<0)return(0);
 if(grantpt(master)<0||unlockpt(master)<0){
  close(master);
  return(0);
 }
 if(!(name=ptsname(master))){
  close(master);
  return(0);
 }
 if((slave=open(name,O_RDWR))==-1){
  close(master);
  return(0);
 }
 if(isastream(slave)){
  if(ioctl(slave,I_PUSH,"ptem")<0||ioctl(slave,I_PUSH,"ldterm")<0){
   close(master);
   close(slave);
   return(0);
  }
 }
 if(v){
  cfgtable.p.merr=master;
  cfgtable.p.serr=slave;
 }
 else{
  cfgtable.p.mout=master;
  cfgtable.p.sout=slave;
 }
#endif
 return(1);
}
#endif
/* prints a formated line to stdout. */
unsigned char cwprintf(char *str){
 signed char x=0;
 unsigned int c=0;
 unsigned int i=0,j=0,k=0;
 size_t p=0;
 char *tmp,*ctmp;
 j=strlen(str);
 k=(8*j);
 if(!(tmp=(char *)malloc((j*k)+j+1)))
  cwexit(1,"malloc() failed.");
 memset(tmp,0,(j*k)+j+1);
 for(k=i=0;j>i;i++){
  if(str[i]=='\\'){
   if(j>=(i+2)){
    if(strchr("\\enrtvxC",str[i+1])){
     if(str[i+1]=='\\')tmp[k++]='\\';
     /* \e is not ANSI-C apparently. */
     else if(str[i+1]=='e')tmp[k++]=0x1b;
     else if(str[i+1]=='n')tmp[k++]='\n';
     else if(str[i+1]=='r')tmp[k++]='\r';
     else if(str[i+1]=='t')tmp[k++]='\t';
     else if(str[i+1]=='v')tmp[k++]='\v';
     else if(str[i+1]=='x'){
      if(j>(i+3)&&isxdigit((unsigned char)str[i+2])
      &&isxdigit((unsigned char)str[i+3])){
       if(sscanf(str+(i+2),"%2x",&c)>0){
        tmp[k++]=c;
        i+=2;
       }
       else{
        tmp[k++]='\\';
        i--;
       }
      }
      else{
       tmp[k++]='\\';
       i--;
      }
     }
     else if(j>=(i+4)&&str[i+1]=='C'&&str[i+2]=='['
     &&(p=(size_t)strchr(str+i+4,']'))){
      p-=((size_t)str+i+3);
      if(p>1){
       if(!(ctmp=(char *)malloc(p+1)))
        cwexit(1,"malloc() failed.");
       memset(ctmp,0,p+1);
       strncpy(ctmp,str+i+3,p);
       if((x=color_atoi(ctmp))>=0){
        strcat(tmp,pal2[x]);
        k+=strlen(pal2[x]);
        i+=(p+2);
       }
       else{
        tmp[k++]='\\';
        i--;
       }
       free(ctmp);
      }
      else{
       tmp[k++]='\\';
       i--;
      }
     }
     else{
      tmp[k++]='\\';
      i--;
     }
     i++;
    }
    else tmp[k++]='\\';
   }
   else tmp[k++]='\\';
  }
  else tmp[k++]=str[i];
 }
 /* using write() so null-bytes can be passed via \x. */
 i=write(STDOUT_FILENO,tmp,k);
 free(tmp);
 return(i==k?0:1);
}
/* sets colorize values. */
void setcolorize(char *str){
 signed char r=0;
 cfgtable.invert=cfgtable.z.on=0;
 cfgtable.z.l=color_atoi(parameter(str,":",0));
 cfgtable.z.h=color_atoi(parameter(str,":",1));
 if(cfgtable.z.l>=0&&cfgtable.z.h>=0)cfgtable.z.on=1;
 else{
  r=color_atoi(str);
  if(r>=0&&r<8){
   cfgtable.z.l=r;
   cfgtable.z.h=(r+8);
  }
  else if(r==8){
   cfgtable.z.l=8;
   cfgtable.z.h=7;
  }
  if(r>=0&&r<9)cfgtable.z.on=1;
 }
 return;
}
/* handles and executes other programs. */
signed char execot(char *prog,unsigned char type,unsigned int l){
 signed char r=0,on=0;
#ifdef HAVE_WAITPID
 signed int e=0;
#endif
 unsigned int i=0,j=0,k=0;
 char *str;
 pid_t p=0;
 k=strlen(prog);
 if(!(str=(char *)malloc(k+strlen(cfgtable.cmdargs)+1)))
  cwexit(1,"malloc() failed.");
 memset(str,0,(k+strlen(cfgtable.cmdargs)+1));
 for(on=j=i=0;k>i;i++){
  if(!on&&!strncmp(prog+i,"{}",2)){
   strcpy(str+j,cfgtable.cmdargs);
   j+=strlen(cfgtable.cmdargs);
   i++;
   on=1;
  }
  else{
   str[j]=prog[i];
   j++;
  }
 }
 str[j]=0;
 switch((p=fork())){
  case -1:
   if(type<2)c_error(l,cfgmsg[36]);
   break;
  case 0:
   if(type==1){
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
   }
   execle("/bin/sh","sh","-c",str,(char *)0,environ);
#ifdef HAVE_WAITPID
  default:
    if(waitpid(p,&e,0)>=0&&WIFEXITED(e))
     r=WEXITSTATUS(e);
    else r=0;
#endif
 }
 free(str);
 return(r);
}
/* handles and executes the desired program. */
noreturn void execcw(signed int oargc,char **oargv,signed int argc,char **argv){
 unsigned char on=0,son=0;
 int i=0,j=0,k=0;
#ifdef HAVE_WAITPID
 signed char re=0;
 signed int e=0;
#endif
 signed int fds[2],fde[2],fdm=0,fd=0,s=0;
 char **nargv,*buf,*tmp;
 fd_set rfds;
#ifdef SIGCHLD
 struct sigaction sa;
#endif
 if(!(cfgtable.m.tot+
 cfgtable.x.tot+
 cfgtable.t.tot+cfgtable.n.on+cfgtable.u.on+cfgtable.l.on))
  cfgtable.nocolor=1;
 if(!cfgtable.nocolor){
#ifndef NO_PTY
  if(cfgtable.pty){
   if(!make_ptypair(0)||!make_ptypair(1))cfgtable.p.on=0;
   else cfgtable.p.on=1;
  }
#endif
  if(pipe(fds)<0)cwexit(1,"pipe() failed.");
  if(pipe(fde)<0)cwexit(1,"pipe() failed.");
  pid_p=getpid();
 }
#ifdef SIGCHLD
 sa.sa_handler=sighandler;
 sigemptyset(&sa.sa_mask);
 sa.sa_flags=SA_NOCLDSTOP;
 if(sigaction(SIGCHLD,&sa,0)<0){
#endif
  signal(SIGTSTP,SIG_IGN);
#ifdef SIGCHLD
  signal(SIGCHLD,sighandler);
 }
#endif
 signal(SIGUSR1,sighandler);
 signal(SIGALRM,sighandler);
 signal(SIGPIPE,sighandler);
 switch(cfgtable.nocolor?0:(pid_c=fork())){
  case -1:
   cwexit(1,"fork() error.");
   break;
  case 0:
   /* child process to execute the program. */
   if(!cfgtable.nocolor){
#ifdef FIONCLEX
    ioctl(fds[0],FIONCLEX,0);
    ioctl(fde[0],FIONCLEX,0);
#endif
#ifndef NO_PTY
    if(dup2((cfgtable.p.on?cfgtable.p.sout:fds[1]),STDOUT_FILENO)<0)
#else
    if(dup2(fds[1],STDOUT_FILENO)<0)
#endif
     cwexit(1,"dup2() failed.");
    close(fds[0]);
    close(fds[1]);
#ifndef NO_PTY
    if(dup2((cfgtable.p.on?cfgtable.p.serr:fde[1]),STDERR_FILENO)<0)
#else
    if(dup2(fde[1],STDERR_FILENO)<0)
#endif
     cwexit(1,"dup2() failed.");
    close(fde[0]);
    close(fde[1]);
   }
#ifdef HAVE_SETSID
   if(!cfgtable.nocolor)setsid();
#endif
   if(cfgtable.cmd)
    execle("/bin/sh",strpname(scrname),"-c",cfgtable.cmd,(char *)0,environ);
   else{
    if(!(nargv=(char **)malloc((sizeof(char *)*argc))))
     cwexit(1,"malloc() failed.");
    nargv[0]=strpname(scrname);
    for(i=2;i<argc;i++)
     nargv[i-1]=argv[i];
    nargv[i-1]=0;
    execve(cfgtable.path,nargv,environ);
   }
   /* shouldn't make it here. (no point to stay alive) */
   exit(1);
   break;
  default:
   /* parent process to read the programs output. (sends INT to child) */
   cfgtable.eint=1;
   signal(SIGINT,sighandler);
#ifndef NO_SETPROCTITLE
   initsetproctitle(oargc,oargv,environ);
   setproctitle("wrapping [%s] {pid=%u}",strpname(scrname),pid_c);
#endif
   if(!(buf=(char *)malloc(BUFSIZE+1)))
    cwexit(1,"malloc() failed.");
   /* minimum catch-up time. */
   if(!cfgtable.w.it_interval.tv_sec&&cfgtable.w.it_interval.tv_usec<100000)
    cfgtable.w.it_interval.tv_usec=cfgtable.w.it_value.tv_usec=100000;
#ifndef NO_PTY
   if(cfgtable.p.on){
    close(fds[0]);
    close(fde[0]);
    fds[0]=cfgtable.p.mout;
    fde[0]=cfgtable.p.merr;
   }
#endif
   fdm=((fds[0]>fde[0]?fds[0]:fde[0])+1);
   while(ext!=2){
    if(ext==1){
     signal(SIGALRM,sighandler);
     setitimer(ITIMER_REAL,&cfgtable.w,0);
    }
    FD_ZERO(&rfds);
    FD_SET(fds[0],&rfds);
    FD_SET(fde[0],&rfds);
    if(select(fdm,&rfds,0,0,0)>=0){
     if(ext==1){
      signal(SIGALRM,SIG_IGN);
      setitimer(ITIMER_REAL,0,0);
     }
     if(FD_ISSET(fds[0],&rfds))fd=fds[0];
     else fd=fde[0];
     memset(buf,0,BUFSIZE);
     if((s=read(fd,buf,BUFSIZE))&&s>0){
      if(!on){
       j=0;
       if(!(tmp=(char *)malloc(s+1)))
        cwexit(1,"malloc() failed.");
       memset(tmp,0,s);
       on=1;
      }
      else{
       if(!(tmp=(char *)realloc(tmp,s+j+1)))
        cwexit(1,"realloc() failed.");
      }
      for(i=0;s>i;i++){
       if(!cfgtable.nostrip&&buf[i]==0x1b&&s>i+3&&buf[i+1]=='['){
        son=0;
        for(k=i+2;!son&&s>k;k++){
         if(buf[k]=='m'){
          if(k-i>2)i+=k-i;
          son=1;
         }
         else if(buf[k]!=';'&&!isdigit((unsigned char)buf[k]))son=1;
        }
       }
       else if(buf[i]=='\n'){
        tmp[j]=0;
        if(cfgtable.col&&cfgtable.col<strlen(tmp))tmp[cfgtable.col]=0;
        fprintf((fd==fds[0]?stdout:stderr),"%s\n",convert_string(tmp));
        fflush(fd==fds[0]?stdout:stderr);
        free(tmp);
        on=0;
        if(s>i){
         if(!(tmp=(char *)malloc(s-i+1)))
          cwexit(1,"malloc() failed.");
         memset(tmp,0,s-i);
         on=1;
        }
        j=0;
       }
       else tmp[j++]=buf[i];
      }
     }
    }
   }
   free(buf);
   if(on)free(tmp);
   rexit=1;
#ifdef HAVE_WAITPID
   if(waitpid(pid_c,&e,WNOHANG)>=0&&WIFEXITED(e))
    re=WEXITSTATUS(e);
   else re=0;
   cwexit(re,0);
#else
   cwexit(0,0);
#endif
   break;
 }
}
#ifndef NO_SETPROCTITLE
#ifndef HAVE_SETPROCTITLE
#ifdef INT_SETPROCTITLE
/* pseudo-setproctitle startup. */
void initsetproctitle(signed int argc,char **argv,char **envp){
 int i=0;
 size_t envpsize=0;
 char *s;
 for(i=0;envp[i]!=0;i++)
  envpsize+=(strlen(envp[i])+1);
 if(!(environ=(char **)malloc((sizeof(char *)*(i+1))+envpsize+1)))
  cwexit(1,"malloc() failed.");
 s=((char *)environ)+((sizeof(char *)*(i+1)));
 for(i=0;envp[i]!=0;i++){
  strcpy(s,envp[i]);
  environ[i]=s;
  s+=(strlen(s)+1);
 }
 environ[i]=0;
 if(!(proct.name=(char *)malloc(strlen(argv[0])+1)))
  cwexit(1,"malloc() failed.");
 strcpy(proct.name,argv[0]);
 proct.argv=argv;
 for(i=0;i<argc;i++){
  if(i==0||proct.largv+1==argv[i])
   proct.largv=(argv[i]+strlen(argv[i]));
 }
 for(i=0;envp[i]!=0;i++){
  if(proct.largv+1==envp[i])
   proct.largv=(envp[i]+strlen(envp[i]));
 }
 return;
}
/* pseudo-setproctitle set. */
void setproctitle(const char *fmt,...){
 unsigned int i;
 char buf[BUFSIZE+1];
 char buf2[BUFSIZE+4+1];
 char *p;
 va_list param;
 va_start(param,fmt);
#if HAVE_VNSPRINTF
 vsnprintf(buf,sizeof(buf),fmt,param);
#else
 vsprintf(buf,fmt,param);
#endif
 va_end(param);
 sprintf(buf2,"cw: %s",buf);
 memset(buf,0,sizeof(buf));
 strncpy(buf,buf2,(sizeof(buf)-1));
 if((i=strlen(buf))>proct.largv-proct.argv[0]-2){
  i=proct.largv-proct.argv[0]-2;
  buf[i]=0;
 }
 strcpy(proct.argv[0],buf);
 p=&proct.argv[0][i];
 while(p<proct.largv)*p++=0;
 proct.argv[1]=0;
 return;
}
#else
void initsetproctitle(signed int argc,char **argv,char **envp){return;}
void setproctitle (const char *fmt,...){return;}
#endif
#else
void initsetproctitle(signed int argc,char **argv,char **envp){return;}
#endif
#endif
/* handles each config file line. (data sizes allocated in c_read()) */
void c_handler(char *line,unsigned int l,signed int argc){
 unsigned char o=0,on=0;
 unsigned int i=0,j=0,k=0,s=0;
 char *tmp,*tmppath,*ptr;
#ifndef HAVE_SETENV
#ifdef HAVE_PUTENV
 char *tmpput;
#endif
#endif
 if(cfgtable.label&&line[0]==':'){
  o=1;
  if(strlen(line)<2||strcmp(cfgtable.label,line+1))cfgtable.iflabel=1;
  else cfgtable.iflabel=cfgtable.iflabelf=0;
 }
 else if(!cfgtable.iflabel&&(!strcmp(parameter(line," ",0),"ifos-else"))){
  o=1;
  if(cfgtable.ifosa)cfgtable.ifos=(cfgtable.ifos?0:1);
  else c_error(l,cfgmsg[31]);
 }
 else if(!cfgtable.iflabel&&(!strcmp(parameter(line," ",0),"ifos")||
 !strcmp(pptr,"ifnos"))){
  cfgtable.ifosa=o=1;
  if(!strcmp(parameter(line," ",1),"-1"))c_error(l,cfgmsg[30]);
  else{
   if(!(tmp=(char *)malloc(strlen(pptr)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,pptr);
   for(j=i=0;!j&&strcmp(parameter(tmp,":",i),"-1");i++){
    if(!strcmp(pptr,"<any>"))j=1;
#ifdef HAVE_UNAME
    else j=(struncmp(pptr)?0:1);
#else
    else j=(strcasecmp(pptr,"<pseudo>")?0:1);
#endif
   }
   free(tmp);
   cfgtable.ifos=j;
   if(!strcmp(parameter(line," ",0),"ifos"))
    cfgtable.ifos=(cfgtable.ifos?0:1);
  }
 }
 else if(!cfgtable.ifos&&!cfgtable.iflabel
 &&(!strcmp(parameter(line," ",0),"ifexit-else"))){
  o=1;
  if(cfgtable.ifexita)cfgtable.ifexit=(cfgtable.ifexit?0:1);
  else c_error(l,cfgmsg[38]);
 }
 else if(!cfgtable.ifos&&!cfgtable.iflabel
 &&(!strcmp(parameter(line," ",0),"ifexit")||!strcmp(pptr,"ifnexit"))){
  cfgtable.ifexita=o=1;
  if(atoi(parameter(line," ",1))>127||atoi(pptr)<-127)
   c_error(l,cfgmsg[37]);
  else{
#ifdef HAVE_WAITPID
   if(!strcmp(pptr,"<any>")||atoi(pptr)==cfgtable.ec)cfgtable.ifexit=1;
#else
   if(!strcmp(pptr,"<any>")||!strcmp(pptr,"<pseudo>"))cfgtable.ifexit=1;
#endif
   if(!strcmp(parameter(line," ",0),"ifexit"))
    cfgtable.ifexit=(cfgtable.ifexit?0:1);
  }
 }
 else if(!cfgtable.ifexit&&!cfgtable.ifos&&!cfgtable.iflabel
 &&(!strcmp(parameter(line," ",0),"ifarg-else"))){
  o=1;
  if(cfgtable.ifarga)cfgtable.ifarg=(cfgtable.ifarg?0:1);
  else c_error(l,cfgmsg[32]);
 }
 else if(!cfgtable.ifexit&&!cfgtable.ifos&&!cfgtable.iflabel
 &&(!strcmp(parameter(line," ",0),"ifarg")||
 !strcmp(pptr,"ifnarg"))){
  cfgtable.ifarga=o=1;
  if(!strcmp(parameter(line," ",1),"-1"))c_error(l,cfgmsg[18]);
  else{
   if(!(tmp=(char *)malloc(strlen(pptr)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,pptr);
   for(j=i=0;!j&&strcmp(parameter(tmp,":",i),"-1");i++){
    if(!strcmp(pptr,"<any>")||(!strcmp(pptr,"<none>")&&argc<3))j=1;
    else j=(regxcmp(cfgtable.cmdargs,pptr,1)?0:1);
   }
   free(tmp);
   cfgtable.ifarg=j;
   if(!strcmp(parameter(line," ",0),"ifarg"))
    cfgtable.ifarg=(cfgtable.ifarg?0:1);
  }
 }
 if(cfgtable.ifos||cfgtable.ifexit||cfgtable.ifarg||cfgtable.iflabel)
  return;
 if(line[0]=='#')return;
 else if(line[0]=='$'){
#ifndef NO_ENVSET
  j=strlen(line);
  for(i=0;j>i;i++)line[i]=line[i+1];
  line=strtok(line,"=");
  if(line&&strlen(line)){
   if(!(tmp=(char *)malloc(strlen(line)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,line);
   line=strtok(0,"");
   if(line&&strlen(line)){
#ifdef HAVE_SETENV
    setenv(tmp,line,1);
#elif HAVE_PUTENV
    if(!(tmpput=(char *)malloc(strlen(tmp)+strlen(line)+2)))
     cwexit(1,"malloc() failed.");
    memset(tmpput,0,(strlen(tmp)+strlen(line)+2));
    sprintf(tmpput,"%s=%s",tmp,line);
    putenv(tmpput);
    /* do not free(tmpput), it is part of the environment. */
#endif
   }
   else c_error(l,cfgmsg[21]);
   free(tmp);
  }
  else c_error(l,cfgmsg[21]);
#endif
 }
 else if(line[0]=='!'||line[0]=='@'){
  if(strlen(line)>1)
   cfgtable.ec=execot(line+1,(line[0]=='!'?0:1),l);
 }
 else if(!strcmp(parameter(line," ",0),"print")){
  ptr=strtok(line," ");
  ptr=strtok(0,"");
  if(ptr&&strlen(ptr)){
   if(cwprintf(ptr))
    c_error(l,cfgmsg[44]);
  }
  else c_error(l,cfgmsg[43]);
 }
 else if(!strcmp(parameter(line," ",0),"path")){
  ptr=strtok(line," ");
  ptr=strtok(0,"");
  if(ptr&&(k=strlen(ptr))){
   s=(getenv("PATH")?strlen(getenv("PATH")):0);
   if(!(tmp=(char *)malloc(k+s+1)))
    cwexit(1,"malloc() failed.");
   memset(tmp,0,(k+s+1));
   for(on=i=j=0;k>i;i++){
    if(!on&&s&&!strncmp(ptr+i,"<env>",5)){
     strcpy(tmp+j,getenv("PATH"));
     j+=s;
     i+=4;
     on=1;
    }
    else{
     tmp[j]=ptr[i];
     j++;
    }
   }
   i=0;
   while(strcmp(parameter(tmp,":",i++),"-1")){
    if(!(tmppath=(char *)malloc(strlen(pptr)+
    strlen(strpname(scrname))+2)))
     cwexit(1,"malloc() failed.");
    sprintf(tmppath,"%s/%s",pptr,strpname(scrname));
    if(!access(tmppath,X_OK)){
     if(cfgtable.path)free(cfgtable.path);
     if(!(cfgtable.path=(char *)malloc(strlen(tmppath)+1)))
      cwexit(1,"malloc() failed.");
     strcpy(cfgtable.path,tmppath);
     free(tmppath);
     break;
    }
    free(tmppath);
   }
   free(tmp);
  }
  if(!cfgtable.path)c_error(l,cfgmsg[10]);
 }
 else if(!strcmp(parameter(line," ",0),"other")){
  ptr=strtok(line," ");
  ptr=strtok(0,"");
  if((k=strlen(ptr))){
   if(cfgtable.cmd)free(cfgtable.cmd);
   if(!(cfgtable.cmd=(char *)malloc(k+strlen(cfgtable.cmdargs)+1)))
    cwexit(1,"malloc() failed.");
   memset(cfgtable.cmd,0,(k+strlen(cfgtable.cmdargs)+1));
   for(on=j=i=0;k>i;i++){
    if(!on&&!strncmp(ptr+i,"{}",2)){
     strcpy(cfgtable.cmd+j,cfgtable.cmdargs);
     j+=strlen(cfgtable.cmdargs);
     i++;
     on=1;
    }
    else{
     cfgtable.cmd[j]=ptr[i];
     j++;
    }
   }
   cfgtable.cmd[j]=0;
  }
  else c_error(l,cfgmsg[20]);
 }
 else if(!strcmp(parameter(line," ",0),"base")){
  if((cfgtable.base=color_atoi(parameter(line," ",1)))<0)
   c_error(l,cfgmsg[11]);
  if(cfgtable.base==16)cfgtable.base=7;
 }
 else if(!strcmp(parameter(line," ",0),"digit")){
  if(strcmp(parameter(line," ",1),"-1")){
   if(!(tmp=(char *)malloc(strlen(pptr)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,pptr);
   if(color_atoi(parameter(tmp,":",0))>-1)
    cfgtable.n.b=color_atoi(parameter(tmp,":",0));
   else{
    c_error(l,cfgmsg[14]);
    cfgtable.n.b=16;
   }
   if(color_atoi(parameter(tmp,":",1))>-1)
    cfgtable.n.a=color_atoi(parameter(tmp,":",1));
   else{
    c_error(l,cfgmsg[14]);
    cfgtable.n.a=16;
   }
   free(tmp);
   cfgtable.n.on=1;
  }
  else c_error(l,cfgmsg[14]);
 }
 else if(!strcmp(parameter(line," ",0),"ucase")){
  if(strcmp(parameter(line," ",1),"-1")){
   if(!(tmp=(char *)malloc(strlen(pptr)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,pptr);
   if(color_atoi(parameter(tmp,":",0))>-1)
    cfgtable.u.b=color_atoi(parameter(tmp,":",0));
   else{
    c_error(l,cfgmsg[16]);
    cfgtable.u.b=16;
   }
   if(color_atoi(parameter(tmp,":",1))>-1)
    cfgtable.u.a=color_atoi(parameter(tmp,":",1));
   else{
    c_error(l,cfgmsg[16]);
    cfgtable.u.a=16;
   }
   free(tmp);
   cfgtable.u.on=1;
  }
  else c_error(l,cfgmsg[16]);
 }
 else if(!strcmp(parameter(line," ",0),"lcase")){
  if(strcmp(parameter(line," ",1),"-1")){
   if(!(tmp=(char *)malloc(strlen(pptr)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,pptr);
   if(color_atoi(parameter(tmp,":",0))>-1)
    cfgtable.l.b=color_atoi(parameter(tmp,":",0));
   else{
    c_error(l,cfgmsg[17]);
    cfgtable.l.b=16;
   }
   if(color_atoi(parameter(tmp,":",1))>-1)
    cfgtable.l.a=color_atoi(parameter(tmp,":",1));
   else{
    c_error(l,cfgmsg[17]);
    cfgtable.l.a=16;
   }
   free(tmp);
   cfgtable.l.on=1;
  }
  else c_error(l,cfgmsg[17]);
 }
 else if(!strcmp(parameter(line," ",0),"match")){
  if(strcmp(parameter(line," ",1),"-1")){
   if(!(tmp=(char *)malloc(strlen(pptr)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,pptr);
   if(color_atoi(parameter(tmp,":",0))>-1)
    cfgtable.m.b[cfgtable.m.cur]=color_atoi(parameter(tmp,":",0));
   else{
    c_error(l,cfgmsg[2]);
    cfgtable.m.b[cfgtable.m.cur]=16;
   }
   if(color_atoi(parameter(tmp,":",1))>-1)
    cfgtable.m.a[cfgtable.m.cur]=color_atoi(parameter(tmp,":",1));
   else{
    c_error(l,cfgmsg[2]);
    cfgtable.m.a[cfgtable.m.cur]=16;
   }
   free(tmp);
   if(strcmp(parameter(line," ",2),"-1")){
    if(!(tmp=(char *)malloc(strlen(line)+1)))
     cwexit(1,"malloc() failed.");
    strcpy(tmp,line);
    ptr=tmp;
    tmp=strtok(tmp," ");
    tmp=strtok(0," ");
    tmp=strtok(0,"");
    for(i=j=0;tmp[i];i++){
     if(!strncmp(tmp+i,"{?}",3)){
      tmp[j++]=-1;
      i+=2;
     }
     else tmp[j++]=tmp[i];
    }
    tmp[j]=0;
    if(cfgtable.m.cur>cfgtable.m.tot)
     c_error(l,cfgmsg[3]);
    else{
     if(!(cfgtable.m.data[cfgtable.m.cur]=(char *)malloc(strlen(tmp)+1)))
      cwexit(1,"malloc() failed.");
     strcpy(cfgtable.m.data[cfgtable.m.cur],tmp);
     cfgtable.m.cur++;
    }
    free(ptr);
   }
   else c_error(l,cfgmsg[4]);
  }
  else c_error(l,cfgmsg[4]);
 }
 else if(!strcmp(parameter(line," ",0),"regex")){
  if(strcmp(parameter(line," ",1),"-1")){
   if(!(tmp=(char *)malloc(strlen(pptr)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,pptr);
   if(color_atoi(parameter(tmp,":",0))>-1)
    cfgtable.x.b[cfgtable.x.cur]=color_atoi(parameter(tmp,":",0));
   else{
    c_error(l,cfgmsg[27]);
    cfgtable.x.b[cfgtable.x.cur]=16;
   }
   if(color_atoi(parameter(tmp,":",1))>-1)
    cfgtable.x.a[cfgtable.x.cur]=color_atoi(parameter(tmp,":",1));
   else{
    c_error(l,cfgmsg[27]);
    cfgtable.x.a[cfgtable.x.cur]=16;
   }
   free(tmp);
   if(strcmp(parameter(line," ",2),"-1")){
    if(!(tmp=(char *)malloc(strlen(line)+1)))
     cwexit(1,"malloc() failed.");
    strcpy(tmp,line);
     ptr=tmp;
    tmp=strtok(tmp," ");
    tmp=strtok(0," ");
    tmp=strtok(0,"");
    if(cfgtable.x.cur>cfgtable.x.tot)
     c_error(l,cfgmsg[28]);
    else{
     if(!(cfgtable.x.data[cfgtable.x.cur]=(char *)malloc(strlen(tmp)+1)))
      cwexit(1,"malloc() failed.");
     strcpy(cfgtable.x.data[cfgtable.x.cur],tmp);
     cfgtable.x.cur++;
    }
    free(ptr);
   }
   else c_error(l,cfgmsg[29]);
  }
  else c_error(l,cfgmsg[29]);
 }
 else if(!strcmp(parameter(line," ",0),"token")){
  if(strcmp(parameter(line," ",1),"-1")){
   if(!(tmp=(char *)malloc(strlen(pptr)+1)))
    cwexit(1,"malloc() failed.");
   strcpy(tmp,pptr);
   if(color_atoi(parameter(tmp,":",0))>-1)
    cfgtable.t.b[cfgtable.t.cur]=color_atoi(parameter(tmp,":",0));
   else{
    c_error(l,cfgmsg[5]);
    cfgtable.t.b[cfgtable.t.cur]=16;
   }
   if(color_atoi(parameter(tmp,":",1))>-1)
    cfgtable.t.a[cfgtable.t.cur]=color_atoi(parameter(tmp,":",1));
   else{
    c_error(l,cfgmsg[5]);
    cfgtable.t.a[cfgtable.t.cur]=16;
   }
   free(tmp);
   if(strcmp(parameter(line," ",2),"-1")){
    if(atoi(pptr)<0||atoi(pptr)>255)
     c_error(l,cfgmsg[6]);
    else{
     cfgtable.t.slot[cfgtable.t.cur]=atoi(pptr);
     if(strcmp(parameter(line," ",3),"-1")){
      if(atoi(pptr)<1||atoi(pptr)>255)
       c_error(l,cfgmsg[7]);
      else{
       if(cfgtable.t.cur>cfgtable.t.tot)
        c_error(l,cfgmsg[8]);
       else{
        cfgtable.t.delim[cfgtable.t.cur]=atoi(pptr);
        cfgtable.t.cur++;
       }
      }
     }
     else c_error(l,cfgmsg[9]);
    }
   }
   else c_error(l,cfgmsg[9]);
  }
  else c_error(l,cfgmsg[9]);
 }
 else if(!strcmp(parameter(line," ",0),"limit")){
  if(!strcmp(parameter(line," ",1),"columns")){
   if(getenv("COLUMNS"))cfgtable.col=atoi(getenv("COLUMNS"));
  }
  else if(atoi(pptr)>-1)cfgtable.col=atoi(pptr);
  else c_error(l,cfgmsg[19]);
 }
 else if(!strcmp(parameter(line," ",0),"wait")){
  if(atoi(parameter(line," ",1))<0)
   c_error(l,cfgmsg[15]);
  else{
   cfgtable.w.it_interval.tv_sec=cfgtable.w.it_value.tv_sec=atoi(pptr);
  if(atoi(parameter(line," ",2))<0)
    c_error(l,cfgmsg[15]);
   else
    cfgtable.w.it_interval.tv_usec=cfgtable.w.it_value.tv_usec=atoi(pptr);
  }
 }
 else if(!strcmp(parameter(line," ",0),"clear"))cfgtable.clear=1;
#ifndef NO_PTY
 else if(!strcmp(parameter(line," ",0),"usepty"))cfgtable.pty=1;
#else
 else if(!strcmp(parameter(line," ",0),"usepty"));
#endif
 else if(!strcmp(parameter(line," ",0),"noextendedregex"))cfgtable.noer=1;
 else if(!strcmp(parameter(line," ",0),"useifregex"))cfgtable.ifregex=1;
 else if(!strcmp(parameter(line," ",0),"nopipe"))cfgtable.nopipe=1;
 else if(!strcmp(parameter(line," ",0),"noeol"))cfgtable.noeol=1;
 else if(!strcmp(parameter(line," ",0),"noaddhelp"))cfgtable.addhelp=0;
 else if(!strcmp(parameter(line," ",0),"nostrip"))cfgtable.nostrip=1;
 else if(!strcmp(parameter(line," ",0),"nocolor"))cfgtable.nocolor=1;
 else if(!strcmp(parameter(line," ",0),"forcecolor"))cfgtable.fc=1;
 else if(!strcmp(parameter(line," ",0),"warnings"))cfgtable.ign=0;
 else if(!strcmp(parameter(line," ",0),"nowarnings"))cfgtable.ign=1;
 else if(!o)c_error(l,cfgmsg[0]);
 return;
}
/* reads (and allocates space) the config file to be passed to c_handler(). */
void c_read(char *file,signed int argc){
 unsigned char i=0;
 unsigned int j=0,k=0;
 char buf[BUFSIZE+1];
 FILE *fs;
 if(!(fs=fopen(file,"r")))cwexit(1,"failed opening config file.");
 for(i=0;i<2;i++){
  /* reset the reading location, it goes through the config file twice. */
  rewind(fs);
  if(i){
   /* build array sizes. */
   if(!(cfgtable.m.data=(char **)malloc(cfgtable.m.tot*sizeof(char *)+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.m.b=(unsigned char *)malloc(cfgtable.m.tot+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.m.a=(unsigned char *)malloc(cfgtable.m.tot+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.x.data=(char **)malloc(cfgtable.x.tot*sizeof(char *)+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.x.b=(unsigned char *)malloc(cfgtable.x.tot+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.x.a=(unsigned char *)malloc(cfgtable.x.tot+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.t.slot=(unsigned char *)malloc(cfgtable.t.tot+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.t.delim=(unsigned char *)malloc(cfgtable.t.tot+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.t.b=(unsigned char *)malloc(cfgtable.t.tot+1)))
    cwexit(1,"malloc() failed.");
   if(!(cfgtable.t.a=(unsigned char *)malloc(cfgtable.t.tot+1)))
    cwexit(1,"malloc() failed.");
  }
  for(memset(buf,0,BUFSIZE);fgets(buf,BUFSIZE,fs);memset(buf,0,BUFSIZE)){
   /* find the amount of definitions to store in memory. */
   if(!i){
    if(!strcmp(parameter(buf," ",0),"match"))cfgtable.m.tot++;
    else if(!strcmp(pptr,"regex"))cfgtable.x.tot++;
    else if(!strcmp(pptr,"token"))cfgtable.t.tot++;
   }
   /* begin actual processing/handling of the config file. (c_handler) */
   else{
    j=strlen(buf);
    if(j>2){
     /* remove excess characters read, ex. \r\n. */
     if(!isprint((unsigned char)buf[j-1]))buf[j-1]=0;
     if(!isprint((unsigned char)buf[j-2]))buf[j-2]=0;
     c_handler(buf,k++,argc);
    }
   }
  }
 }
 fclose(fs);
 if(cfgtable.base<0)cfgtable.base=7;
 cfgtable.m.tot=cfgtable.m.cur;
 cfgtable.x.tot=cfgtable.x.cur;
 cfgtable.t.tot=cfgtable.t.cur;
 if(!cfgtable.path&&!cfgtable.cmd)c_error(0,cfgmsg[1]);
 else if(cfgtable.path&&cfgtable.cmd)c_error(0,cfgmsg[22]);
 else if(cfgtable.iflabelf)c_error(0,cfgmsg[26]);
 else return;
 cwexit(1,cfgmsg[46]);
 return;
}
/* this get appended to "--help" displays/ */
static void addhelp_display(void){
 fprintf(stdout,"\n%s\n","color wrapper (cw) options:");
 /* using spaces instead of /t to deal with terminal emulation issues. */
 fprintf(stdout,"%s\n","  +co, --cw-colorize=color[:color]  sets colors to the provided argument(s"
 ").");
 fprintf(stdout,"%s\n","  +iv, --cw-invert                  invert the internal colormap.");
 fprintf(stdout,"%s\n","  +nc, --cw-nocolor                 disable color wrapping of this"
 " program.");
#ifndef NO_PTY
 fprintf(stdout,"%s\n","  +py, --cw-usepty                  allocates a pseudo terminal.");
#endif
 return;
}
/* configuration error message. */
void c_error(unsigned int l,const char *text){
 if(!cfgtable.ign)
  fprintf(stdout,"cw:definition_error:%u: %s\n",l,text);
 return;
}
/* exit with or without a reason, resets color too. */
noreturn void cwexit(signed char level,const char *reason){
 if(!rexit&&level)fprintf(stdout,"cw:exit: %s\n",reason);
 else if(cfgtable.addhelp&&!cfgtable.nocolor&&!cfgtable.cmd&&!cfgtable.po)
  addhelp_display();
 fflush(stdout);
 exit(level);
}
