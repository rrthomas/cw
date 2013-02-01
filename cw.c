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
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif
#include <fcntl.h>
#ifdef HAVE_PTY_H
#include <pty.h>
#endif
#include <ctype.h>
#include <regex.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>

#ifndef HAVE_OPENPTY
#define NO_PTY
#endif
#if !defined(HAVE_SETPROCTITLE) && (defined(__APPLE_CC__) || defined(__linux__))
#define INT_SETPROCTITLE
#define HAVE_SETPROCTITLE
#endif

/* prototypes. */
void sighandler(signed int);
char *strpname(char *);
unsigned char strwcmp(char *,char *);
unsigned char struncmp(char *);
signed char color_atoi(char *);
signed char make_ptypair(unsigned char v);
unsigned char cwprintf(char *);
void setcolorize(char *);
signed char execot(char *,unsigned char,unsigned int);
void execcw(signed int,char **,signed int,char **);
#ifdef INT_SETPROCTITLE
void initsetproctitle(signed int,char **,char **);
void setproctitle(const char *,...);
#endif
void c_handler(char *,unsigned int,signed int);
void c_read(char *,signed int);
void c_error(unsigned int,const char *);
void cwexit(signed char,const char *);

/* pseudo-setproctitle table. */
#ifdef INT_SETPROCTITLE
struct{
 char **argv;
 char *largv;
 char *name;
}proct;
#endif
/* configuration table. */
struct{
 unsigned char col;
 signed char addhelp;
 signed char ifarg;
 signed char ifarga;
 signed char ifexit;
 signed char ifexita;
 signed char ifos;
 signed char ifosa;
 signed char base;
 signed char ec;
 signed char eint;
 signed char fc;
 signed char invert;
 signed char nocolor;
 signed char noeol;
 signed char nostrip;
 signed char ron;
 char *path;
 char *cmd;
 char *cmdargs;
#ifdef HAVE_SETPROCTITLE
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
}cfgtable;

char id[]="$Id: cw.c,v "VERSION" v9/fakehalo Exp $";

unsigned char ext=0,rexit=0;
char *pal2[18],*aptr,*fptr,*pptr,*progname,*scrname;
pid_t pid_p,pid_c;
extern char **environ;

#define BUFSIZE 1024

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
 "NO SUCH ERROR",
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

static void *cwmalloc(size_t n) {
 void *p = calloc(1, n);
 if (!p) cwexit(1,"malloc() failed.");
 return p;
}

/* program start. */
signed int main(signed int argc,char **argv){
 int i=0,j=0, margc=0;
 char *ptr,**margv;
 cfgtable.z.l=cfgtable.z.h=-1;
 margv=(char **)cwmalloc((sizeof(char *)*(argc+1)));
 margv[0]=argv[0];
 for(margc=i=1;i<argc;i++){
  if(!strcmp("--cw-nocolor",argv[i]))
   cfgtable.nocolor=1;
  else if(!strcmp("--cw-invert",argv[i]))
   cfgtable.invert=1;
  else if(!strncmp("--cw-colorize=",argv[i],14)){
   if(strlen(argv[i])>14)setcolorize(argv[i]+14);
  }
  else if(!strcmp("--help",argv[i])){
   cfgtable.addhelp=1;
  }
  else if(!strcmp("--version",argv[i])){
   cwexit(1,"cw (color wrapper) v"VERSION" (features="
#ifndef NO_PTY
    "pty"
#endif
#ifdef HAVE_SETPROCTITLE
    "setproctitle"
#endif
    ")");
   }
  else margv[margc++]=argv[i];
 }
 margv[margc]=0;
 progname=(char *)cwmalloc(strlen(margv[0])+1);
 strcpy(progname,margv[0]);
 for(i=2;margc>i;i++)j+=(strlen(margv[i])+1);
 cfgtable.cmdargs=(char *)cwmalloc(j+1);
 j=0;
 for(i=2;margc>i;i++){
  sprintf(cfgtable.cmdargs+j,"%s%c",margv[i],(margc-i==1?0:32));
  j+=(strlen(margv[i])+1);
 }
 if(margc>1){
  if(access(margv[1],F_OK))
   cwexit(1,"non-existent path to definition file.");
  scrname=(char *)cwmalloc(strlen(margv[1])+1);
  strcpy(scrname,margv[1]);
 }
 else {
  cfgtable.addhelp=1;
  cwexit(0,0);
 }
 for(i=0;18>i;i++){
  pal2[i]=(char *)cwmalloc(strlen(pal2_orig[i])+1);
  strcpy(pal2[i],pal2_orig[i]);
 }
 if(getenv("CW_CHK_SETCODE"))
  cfgtable.ec=execot(getenv("CW_CHK_SETCODE"),2,0);
 cfgtable.base=-1;
 cfgtable.col=cfgtable.ifarg=cfgtable.ifarga=0;
 cfgtable.ifos=cfgtable.ifosa=cfgtable.ifexit=cfgtable.ifexita=0;
 cfgtable.ron=cfgtable.m.cur=cfgtable.t.cur=0;
#ifndef NO_PTY
 cfgtable.p.on=0;
#endif
 if(!cfgtable.z.on&&(ptr=(char *)getenv("CW_COLORIZE")))
  setcolorize(ptr);
 if(getenv("CW_INVERT"))cfgtable.invert=1;
 c_read(scrname,margc);
 cfgtable.nocolor+=(getenv("NOCOLOR")?1:0);
 cfgtable.nocolor+=(getenv("MAKELEVEL")?1:0);
 if(!cfgtable.nocolor&&getenv("CW_CHK_NOCOLOR"))
  cfgtable.nocolor=(execot(getenv("CW_CHK_NOCOLOR"),2,0)?1:0);
 if(getenv("NOCOLOR_NEXT")){
  setenv("NOCOLOR","1",1);
  unsetenv("NOCOLOR_NEXT");
 }
 /* from patch submitted by <komar@ukr.net>. (modified from original) */
 if(!isatty(STDOUT_FILENO)||!isatty(STDERR_FILENO))
  cfgtable.nocolor=1;
 if(cfgtable.z.on)cfgtable.invert=0;
 if(cfgtable.fc&&cfgtable.nocolor)cfgtable.nocolor=0;
 execcw(argc,argv,margc,margv);
 cwexit(0,0);
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
 else if(sig==SIGPIPE||sig==SIGINT){
  fprintf(stderr,"%s",pal2[16]);
  fflush(stderr);
  cwexit(0,0);
 }
}
/* plucks a requested token out of a string. */
static char *parameter(const char *string,const char *delim,unsigned int p){
 unsigned int n=p;
 char *arg;
 free(fptr);
 fptr=arg=(char *)cwmalloc(strlen(string)+1);
 strcpy(arg,string);
 arg=strtok(arg,delim);
 while(n&&(arg=strtok(0,delim)))n--;
 if(!arg){
  free(fptr);
  fptr=arg=(char *)cwmalloc(3);
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
 tbuf=(char *)cwmalloc(s+1);
 strcpy(tbuf,line);
 /* start processing the 'digit' definition. */
 if(cfgtable.n.on){
  tmp=(char *)cwmalloc(s*16+1);
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
  tbuf=(char *)cwmalloc(s+1);
  strcpy(tbuf,tmp);
  free(tmp);
  on=0;
 }
 /* start processing the 'ucase' definition. */
 if(cfgtable.u.on){
  tmp=(char *)cwmalloc(s*16+1);
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
  tbuf=(char *)cwmalloc(s+1);
  strcpy(tbuf,tmp);
  free(tmp);
  on=0;
 }
 /* start processing the 'lcase' definition. */
 if(cfgtable.l.on){
  tmp=(char *)cwmalloc(s*16+1);
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
  tbuf=(char *)cwmalloc(s+1);
  strcpy(tbuf,tmp);
  free(tmp);
  on=0;
 }
 /* start processing the 'token' definitions. */
 for(i=0;i<cfgtable.t.tot;i++){
  s=strlen(tbuf);
  tmp=(char *)cwmalloc(s+strlen(tbuf)+16+1);
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
  tbuf=(char *)cwmalloc(strlen(tmp)+1);
  strcpy(tbuf,tmp);
  free(tmp);
 }
 /* start processing the 'match' definitions. */
 if(cfgtable.m.tot){
  s=strlen(tbuf);
  tmp=(char *)cwmalloc(s*(cfgtable.m.tot*16+1)+strlen(tbuf)+1);
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
  tbuf=(char *)cwmalloc(strlen(tmp)+1);
  strcpy(tbuf,tmp);
  free(tmp);
 }
 /* start processing the 'regex' definitions. */
 if(cfgtable.x.tot){
  for(j=i=0;i<cfgtable.x.tot;i++){
   s=strlen(tbuf);
   tmp=(char *)cwmalloc(s*(cfgtable.x.tot*16+1)+s+1);
   on=j=l=k=0;
   if(regcomp(&re,cfgtable.x.data[i],REG_EXTENDED))
    free(tmp);
   else{
    while(k<s&&!regexec(&re,tbuf+k,1,&pm,(k?REG_NOTBOL:0))){
     if(pm.rm_so){
      tmpcmp=(char *)cwmalloc(pm.rm_so+1);
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
     tmpcmp=(char *)cwmalloc(l+1);
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
    tbuf=(char *)cwmalloc(strlen(tmp)+1);
    strcpy(tbuf,tmp);
    free(tmp);
    on=0;
   }
  }
 }
 buf=(char *)cwmalloc(strlen(pal2[cfgtable.base])+strlen(tbuf)
                      +cfgtable.b.llen+cfgtable.b.rlen+(!cfgtable.noeol?4:0)+1);
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
/* checks for a regex match of a string. */
static unsigned char regxcmp(char *str,char *pattern){
 signed int r=0;
 regex_t re;
 if(regcomp(&re,pattern,REG_EXTENDED|REG_NOSUB))
  return(1);
 r=regexec(&re,str,0,0,0);
 regfree(&re);
 return r==REG_NOMATCH;
}
/* scans for a system name match. */
unsigned char struncmp(char *cmp){
 struct utsname un;
 if(uname(&un)<0||!strlen(un.sysname))return(1);
 return regxcmp(un.sysname,cmp);
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
 if(v){
  if(openpty(&cfgtable.p.merr,&cfgtable.p.serr,0,0,0))return(0);
 }
 else{
  if(openpty(&cfgtable.p.mout,&cfgtable.p.sout,0,0,0))return(0);
 }
 return(1);
}
#endif
/* prints a formatted line to stdout. */
unsigned char cwprintf(char *str){
 signed char x=0;
 unsigned int c=0;
 unsigned int i=0,j=0,k=0;
 size_t p=0;
 char *tmp,*ctmp;
 j=strlen(str);
 k=(8*j);
 tmp=(char *)cwmalloc((j*k)+j+1);
 for(k=i=0;j>i;i++){
  if(str[i]=='\\'){
   if(j>=(i+2)){
    if(strchr("\\enrtvxC",str[i+1])){
     if(str[i+1]=='\\')tmp[k++]='\\';
     /* \e is not standard C. */
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
       ctmp=(char *)cwmalloc(p+1);
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
}
/* handles and executes other programs. */
signed char execot(char *prog,unsigned char type,unsigned int l){
 signed char r=0,on=0;
 signed int e=0;
 unsigned int i=0,j=0,k=0;
 char *str;
 pid_t p=0;
 k=strlen(prog);
 str=(char *)cwmalloc(k+strlen(cfgtable.cmdargs)+1);
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
  default:
    if(waitpid(p,&e,0)>=0&&WIFEXITED(e))
     r=WEXITSTATUS(e);
    else r=0;
 }
 free(str);
 return(r);
}
/* handles and executes the desired program. */
noreturn void execcw(signed int oargc,char **oargv,signed int argc,char **argv){
 unsigned char on=0,son=0;
 int i=0,j=0,k=0;
 signed char re=0;
 signed int fds[2],fde[2],fdm=0,fd=0,s=0,e=0;
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
  if(!make_ptypair(0)||!make_ptypair(1))cfgtable.p.on=0;
  else cfgtable.p.on=1;
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
    nargv=(char **)cwmalloc((sizeof(char *)*argc));
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
#ifdef HAVE_SETPROCTITLE
#ifdef INT_SETPROCTITLE
   initsetproctitle(oargc,oargv,environ);
#endif
   setproctitle("wrapping [%s] {pid=%u}",strpname(scrname),pid_c);
#endif
   buf=(char *)cwmalloc(BUFSIZE+1);
#ifndef NO_PTY
   if(cfgtable.p.on){
    close(fds[0]);
    close(fde[0]);
    fds[0]=cfgtable.p.mout;
    fde[0]=cfgtable.p.merr;
   }
#endif
   fdm=((fds[0]>fde[0]?fds[0]:fde[0])+1);
   while(ext!=1){
    FD_ZERO(&rfds);
    FD_SET(fds[0],&rfds);
    FD_SET(fde[0],&rfds);
    if(select(fdm,&rfds,0,0,0)>=0){
     if(FD_ISSET(fds[0],&rfds))fd=fds[0];
     else fd=fde[0];
     memset(buf,0,BUFSIZE);
     if((s=read(fd,buf,BUFSIZE))&&s>0){
      if(!on){
       j=0;
       tmp=(char *)cwmalloc(s+1);
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
         tmp=(char *)cwmalloc(s-i+1);
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
   if(waitpid(pid_c,&e,WNOHANG)>=0&&WIFEXITED(e))
    re=WEXITSTATUS(e);
   else re=0;
   cwexit(re,0);
   break;
 }
}
#ifdef INT_SETPROCTITLE
/* initialize pseudo-setproctitle. */
void initsetproctitle(signed int argc,char **argv,char **envp){
 int i=0;
 size_t envpsize=0;
 char *s;
 for(i=0;envp[i]!=0;i++)
  envpsize+=(strlen(envp[i])+1);
 environ=(char **)cwmalloc((sizeof(char *)*(i+1))+envpsize+1);
 s=((char *)environ)+((sizeof(char *)*(i+1)));
 for(i=0;envp[i]!=0;i++){
  strcpy(s,envp[i]);
  environ[i]=s;
  s+=(strlen(s)+1);
 }
 environ[i]=0;
 proct.name=(char *)cwmalloc(strlen(argv[0])+1);
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
}
/* pseudo-setproctitle. */
void setproctitle(const char *fmt,...){
 unsigned int i;
 char buf[BUFSIZE+1];
 char buf2[BUFSIZE+4+1];
 char *p;
 va_list param;
 va_start(param,fmt);
 vsnprintf(buf,sizeof(buf),fmt,param);
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
}
#endif
/* handles each config file line. (data sizes allocated in c_read()) */
void c_handler(char *line,unsigned int l,signed int argc){
 unsigned char o=0,on=0;
 unsigned int i=0,j=0,k=0,s=0;
 char *tmp,*tmppath,*ptr;
 if(!strcmp(parameter(line," ",0),"ifos-else")){
  o=1;
  if(cfgtable.ifosa)cfgtable.ifos=(cfgtable.ifos?0:1);
  else c_error(l,cfgmsg[31]);
 }
 else if(!strcmp(parameter(line," ",0),"ifos")||!strcmp(pptr,"ifnos")){
  cfgtable.ifosa=o=1;
  if(!strcmp(parameter(line," ",1),"-1"))c_error(l,cfgmsg[30]);
  else{
   tmp=(char *)cwmalloc(strlen(pptr)+1);
   strcpy(tmp,pptr);
   for(j=i=0;!j&&strcmp(parameter(tmp,":",i),"-1");i++){
    if(!strcmp(pptr,"<any>"))j=1;
    else j=(struncmp(pptr)?0:1);
   }
   free(tmp);
   cfgtable.ifos=j;
   if(!strcmp(parameter(line," ",0),"ifos"))
    cfgtable.ifos=(cfgtable.ifos?0:1);
  }
 }
 else if(!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifexit-else"))){
  o=1;
  if(cfgtable.ifexita)cfgtable.ifexit=(cfgtable.ifexit?0:1);
  else c_error(l,cfgmsg[38]);
 }
 else if(!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifexit")||!strcmp(pptr,"ifnexit"))){
  cfgtable.ifexita=o=1;
  if(atoi(parameter(line," ",1))>127||atoi(pptr)<-127)
   c_error(l,cfgmsg[37]);
  else{
   if(!strcmp(pptr,"<any>")||atoi(pptr)==cfgtable.ec)cfgtable.ifexit=1;
   if(!strcmp(parameter(line," ",0),"ifexit"))
    cfgtable.ifexit=(cfgtable.ifexit?0:1);
  }
 }
 else if(!cfgtable.ifexit&&!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifarg-else"))){
  o=1;
  if(cfgtable.ifarga)cfgtable.ifarg=(cfgtable.ifarg?0:1);
  else c_error(l,cfgmsg[32]);
 }
 else if(!cfgtable.ifexit&&!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifarg")||
 !strcmp(pptr,"ifnarg"))){
  cfgtable.ifarga=o=1;
  if(!strcmp(parameter(line," ",1),"-1"))c_error(l,cfgmsg[18]);
  else{
   tmp=(char *)cwmalloc(strlen(pptr)+1);
   strcpy(tmp,pptr);
   for(j=i=0;!j&&strcmp(parameter(tmp,":",i),"-1");i++){
    if(!strcmp(pptr,"<any>")||(!strcmp(pptr,"<none>")&&argc<3))j=1;
    else j=!regxcmp(cfgtable.cmdargs,pptr);
   }
   free(tmp);
   cfgtable.ifarg=j;
   if(!strcmp(parameter(line," ",0),"ifarg"))
    cfgtable.ifarg=(cfgtable.ifarg?0:1);
  }
 }
 if(cfgtable.ifos||cfgtable.ifexit||cfgtable.ifarg)
  return;
 if(line[0]=='#')return;
 else if(line[0]=='$'){
  j=strlen(line);
  for(i=0;j>i;i++)line[i]=line[i+1];
  line=strtok(line,"=");
  if(line&&strlen(line)){
   tmp=(char *)cwmalloc(strlen(line)+1);
   strcpy(tmp,line);
   line=strtok(0,"");
   if(line&&strlen(line))
    setenv(tmp,line,1);
   else c_error(l,cfgmsg[21]);
   free(tmp);
  }
  else c_error(l,cfgmsg[21]);
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
   tmp=(char *)cwmalloc(k+s+1);
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
    tmppath=(char *)cwmalloc(strlen(pptr)+strlen(strpname(scrname))+2);
    sprintf(tmppath,"%s/%s",pptr,strpname(scrname));
    if(!access(tmppath,X_OK)){
     if(cfgtable.path)free(cfgtable.path);
     cfgtable.path=(char *)cwmalloc(strlen(tmppath)+1);
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
   cfgtable.cmd=(char *)cwmalloc(k+strlen(cfgtable.cmdargs)+1);
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
   tmp=(char *)cwmalloc(strlen(pptr)+1);
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
   tmp=(char *)cwmalloc(strlen(pptr)+1);
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
   tmp=(char *)cwmalloc(strlen(pptr)+1);
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
   tmp=(char *)cwmalloc(strlen(pptr)+1);
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
    tmp=(char *)cwmalloc(strlen(line)+1);
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
     cfgtable.m.data[cfgtable.m.cur]=(char *)cwmalloc(strlen(tmp)+1);
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
   tmp=(char *)cwmalloc(strlen(pptr)+1);
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
    tmp=(char *)cwmalloc(strlen(line)+1);
    strcpy(tmp,line);
     ptr=tmp;
    tmp=strtok(tmp," ");
    tmp=strtok(0," ");
    tmp=strtok(0,"");
    if(cfgtable.x.cur>cfgtable.x.tot)
     c_error(l,cfgmsg[28]);
    else{
     cfgtable.x.data[cfgtable.x.cur]=(char *)cwmalloc(strlen(tmp)+1);
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
   tmp=(char *)cwmalloc(strlen(pptr)+1);
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
 else if(!strcmp(parameter(line," ",0),"noeol"))cfgtable.noeol=1;
 else if(!strcmp(parameter(line," ",0),"noaddhelp"))cfgtable.addhelp=0;
 else if(!strcmp(parameter(line," ",0),"nostrip"))cfgtable.nostrip=1;
 else if(!strcmp(parameter(line," ",0),"nocolor"))cfgtable.nocolor=1;
 else if(!strcmp(parameter(line," ",0),"forcecolor"))cfgtable.fc=1;
 else if(!o)c_error(l,cfgmsg[0]);
}
/* reads (and allocates space for) the config file to be passed to c_handler(). */
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
   cfgtable.m.data=(char **)cwmalloc(cfgtable.m.tot*sizeof(char *)+1);
   cfgtable.m.b=(unsigned char *)cwmalloc(cfgtable.m.tot+1);
   cfgtable.m.a=(unsigned char *)cwmalloc(cfgtable.m.tot+1);
   cfgtable.x.data=(char **)cwmalloc(cfgtable.x.tot*sizeof(char *)+1);
   cfgtable.x.b=(unsigned char *)cwmalloc(cfgtable.x.tot+1);
   cfgtable.x.a=(unsigned char *)cwmalloc(cfgtable.x.tot+1);
   cfgtable.t.slot=(unsigned char *)cwmalloc(cfgtable.t.tot+1);
   cfgtable.t.delim=(unsigned char *)cwmalloc(cfgtable.t.tot+1);
   cfgtable.t.b=(unsigned char *)cwmalloc(cfgtable.t.tot+1);
   cfgtable.t.a=(unsigned char *)cwmalloc(cfgtable.t.tot+1);
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
 else return;
 cwexit(1,cfgmsg[46]);
}
/* this gets appended to "--help" displays. */
static void addhelp_display(void){
 fprintf(stdout,"\n%s\n","color wrapper (cw) options:");
 /* using spaces instead of \t to deal with terminal emulation issues. */
 fprintf(stdout,"%s\n","  --cw-colorize=color[:color]  sets colors to the provided argument(s).");
 fprintf(stdout,"%s\n","  --cw-invert                  invert the internal colormap.");
 fprintf(stdout,"%s\n","  --cw-nocolor                 disable color wrapping of this program.");
}
/* configuration error message. */
void c_error(unsigned int l,const char *text){
 fprintf(stdout,"cw:definition_error:%u: %s\n",l,text);
}
/* exit with or without a reason, resets color too. */
noreturn void cwexit(signed char level,const char *reason){
 if(!rexit&&level)fprintf(stdout,"cw:exit: %s\n",reason);
 else if(cfgtable.addhelp&&!cfgtable.nocolor&&!cfgtable.cmd)
  addhelp_display();
 fflush(stdout);
 exit(level);
}
