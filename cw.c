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
#include <stdbool.h>
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
#include "progname.h"
#include "dirname.h"
#include "gl_xlist.h"
#include "gl_linked_list.h"
#include "xalloc.h"

#define BUFSIZE 1024

#ifndef HAVE_OPENPTY
#define NO_PTY
#endif
#if !defined(HAVE_SETPROCTITLE) && (defined(__APPLE_CC__) || defined(__linux__))
#define INT_SETPROCTITLE
#define HAVE_SETPROCTITLE
#endif
#ifdef INT_SETPROCTITLE
struct{
 char **argv;
 char *largv;
 char *name;
}proct;
/* initialize pseudo-setproctitle. */
static void initsetproctitle(int argc,char **argv,char **envp){
 int i=0;
 size_t envpsize=0;
 char *s;
 for(i=0;envp[i]!=0;i++)
  envpsize+=(strlen(envp[i])+1);
 environ=(char **)xzalloc((sizeof(char *)*(i+1))+envpsize+1);
 s=((char *)environ)+((sizeof(char *)*(i+1)));
 for(i=0;envp[i]!=0;i++){
  strcpy(s,envp[i]);
  environ[i]=s;
  s+=(strlen(s)+1);
 }
 environ[i]=0;
 proct.name=xstrdup(argv[0]);
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
static void setproctitle(const char *fmt,...){
 size_t i;
 char buf[BUFSIZE+1];
 char *p;
 va_list param;
 va_start(param,fmt);
 vsnprintf(buf,sizeof(buf),fmt,param);
 va_end(param);
 if((i=strlen(buf))>(size_t)(proct.largv-proct.argv[0]-2)){
  i=proct.largv-proct.argv[0]-2;
  buf[i]='\0';
 }
 strcpy(proct.argv[0],buf);
 p=&proct.argv[0][i];
 while(p<proct.largv)*p++=0;
 proct.argv[1]=0;
}
#endif

/* configuration table. */
struct{
 bool ifarg, ifarga;
 bool ifos, ifosa;
 bool base;
 bool ec;
 bool eint;
 bool invert;
 bool nocolor, nocolor_stdout, nocolor_stderr;
 char *cmd, *cmdargs;
#ifdef HAVE_SETPROCTITLE
 char *title;
#endif
 gl_list_t m;
 struct{
  signed char l;
  signed char h;
  bool on;
 }z;
#ifndef NO_PTY
 struct{
  int mout;
  int sout;
  int merr;
  int serr;
  bool on;
 }p;
#endif
}cfgtable;

/* match instruction. */
typedef struct{
  char *data;
  unsigned char b;
  unsigned char a;
}match;

char id[]="$Id: cw.c,v "VERSION" v9/fakehalo Exp $";

static bool ext=false;
static unsigned char rexit=0;
static char *pal2[18],*aptr,*fptr,*pptr,*scrname,*base_scrname;
static pid_t pid_c;
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

/* configuration error message. */
void c_error(size_t l,const char *text){
 fprintf(stdout,"cw:definition_error:%zu: %s\n",l,text);
}

/* exit with or without a reason, resets color too. */
noreturn void cwexit(signed char level,const char *reason){
 if(!rexit&&level)fprintf(stdout,"cw:exit: %s\n",reason);
 fflush(stdout);
 exit(level);
}

void xalloc_die(void) {
 cwexit(1,"malloc() failed.");
}

/* checks for a regex match of a string. */
static bool regxcmp(char *str,char *pattern){
 int r;
 regex_t re;
 if(regcomp(&re,pattern,REG_EXTENDED|REG_NOSUB))
  return(1);
 r=regexec(&re,str,0,0,0);
 regfree(&re);
 return r==REG_NOMATCH;
}

/* scans for a system name match. */
static bool struncmp(char *cmp){
 struct utsname un;
 if(uname(&un)<0||!strlen(un.sysname))return(1);
 return regxcmp(un.sysname,cmp);
}

/* plucks a requested token out of a string. */
static char *parameter(const char *string,const char *delim,size_t p){
 size_t n=p;
 char *arg;
 free(fptr);
 fptr=arg=xstrdup(string);
 arg=strtok(arg,delim);
 while(n&&(arg=strtok(0,delim)))n--;
 if(!arg){
  free(fptr);
  fptr=arg=(char *)xzalloc(3);
  arg=NULL;
 }
 return(pptr=arg);
}

/* filter a directory out of a PATH-like colon-separated path list. */
static char *remove_dir_from_path(const char *path, const char *dir){
 if(path){
  char *canon_dir=canonicalize_file_name(dir);
  size_t s=strlen(path),i=0;
  char *newpath=(char *)xzalloc(s+1);
  char *tmp=xstrdup(path);
  while(parameter(tmp,":",i++)){
   char *canon_pptr=canonicalize_file_name(pptr);
   if(strcmp(canon_pptr,canon_dir)){
    if(*newpath)
     strcat(newpath,":");
    strcat(newpath,pptr);
   }
   free(canon_pptr);
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

/* converts the color string to a numerical storage value. (0-17) */
static _GL_ATTRIBUTE_PURE signed char color_atoi(char *color){
 signed char i=0;
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

/* sets colorize values. */
static void setcolorize(char *str){
 signed char r=0;
 cfgtable.invert=cfgtable.z.on=false;
 cfgtable.z.l=color_atoi(parameter(str,":",0));
 cfgtable.z.h=color_atoi(parameter(str,":",1));
 if(cfgtable.z.l>=0&&cfgtable.z.h>=0)cfgtable.z.on=true;
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
  if(r>=0&&r<9)cfgtable.z.on=true;
 }
}

/* converts the original string to a color string based on the config file. */
static char *convert_string(const char *line){
 gl_list_iterator_t i;
 size_t j=0,k=0,l=0;
 const match *m;
 char *buf;
 regex_t re;
 regmatch_t pm;
 char *tbuf=xstrdup(line);
 /* start processing the 'match' definitions. */
 for(j=0,i=gl_list_iterator(cfgtable.m);gl_list_iterator_next(&i,(const void **)&m,NULL);){
  size_t s=strlen(tbuf);
  bool on=false;
  j=l=k=0;
  if(!regcomp(&re,m->data,REG_EXTENDED)){
   char *tmpcmp,*tmp=(char *)xzalloc(s*(gl_list_size(cfgtable.m)*16+1)+s+1);
   while(k<s&&!regexec(&re,tbuf+k,1,&pm,(k?REG_NOTBOL:0))){
    if(pm.rm_so){
     tmpcmp=(char *)xzalloc(pm.rm_so+1);
     strncpy(tmpcmp,tbuf+k,pm.rm_so);
     strcpy(tmp+j,tmpcmp);
     j+=strlen(tmpcmp);
     free(tmpcmp);
    }
    if(!pm.rm_so&&!pm.rm_eo){
     pm.rm_eo++;
     on=true;
    }
    l=(pm.rm_eo-pm.rm_so);
    tmpcmp=(char *)xzalloc(l+1);
    k+=pm.rm_so;
    strncpy(tmpcmp,tbuf+k,l);
    if(!on&&!memchr(tbuf+(k-(k<7?k:7)),'\x1b',(k<7?k:7))&&m->b!=17){
     strcpy(tmp+j,pal2[m->b==16?cfgtable.base:m->b]);
     j+=strlen(pal2[m->b==16?cfgtable.base:m->b]);
    }
    strcpy(tmp+j,tmpcmp);
    j+=strlen(tmpcmp);
    free(tmpcmp);
    if(!on&&!memchr(tbuf+(k-(k<7?k:7)),'\x1b',(k<7?k:7))&&m->a!=17){
     strcpy(tmp+j,pal2[m->a==16?cfgtable.base:m->a]);
     j+=strlen(pal2[m->a==16?cfgtable.base:m->a]);
    }
    on=false;
    k-=pm.rm_so;
    k+=pm.rm_eo;
   }
   regfree(&re);
   if(s>k)strcpy(tmp+j,tbuf+k);
   free(tbuf);
   tbuf=tmp;
   on=false;
  }
 }
 buf=(char *)xzalloc(strlen(pal2[cfgtable.base])+strlen(tbuf)+4+1);
 sprintf(buf,"%s%s%s",pal2[cfgtable.base],tbuf,pal2[16]);
 free(tbuf);
 free(aptr);
 return(aptr=buf);
}

#ifndef NO_PTY
/* creates a pty pair. (master/slave) */
static signed char make_ptypair(unsigned char v){
 if(v){
  if(openpty(&cfgtable.p.merr,&cfgtable.p.serr,0,0,0))return(0);
 }
 else{
  if(openpty(&cfgtable.p.mout,&cfgtable.p.sout,0,0,0))return(0);
 }
 return(1);
}
#endif

/* all-purpose signal handler. */
static void sighandler(int sig){
 if(sig==SIGINT&&cfgtable.eint){
  if(pid_c){
   kill(pid_c,SIGINT);
   cfgtable.eint=false;
  }
 }
#ifdef SIGCHLD
 else if(sig==SIGCHLD)ext=true;
#endif
 if(sig==SIGPIPE||sig==SIGINT){
  fprintf(stderr,"%s",pal2[16]);
  fflush(stderr);
  cwexit(0,0);
 }
}

static void sig_catch(int sig, void (*handler)(int))
{
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(sig, &sa, 0);         /* XXX ignores errors */
}


/* handles and executes the desired program. */
noreturn void execcw(int argc,char **argv){
 bool on=false,son=false;
 ssize_t i=0,j=0,k=0,s=0;
 signed char re=0;
 int fds[2],fde[2],fdm=0,fd=0,e=0;
 char *buf,*tmp;
 fd_set rfds;
#ifdef SIGCHLD
 struct sigaction sa;
#endif
 if(!(cfgtable.m))
  cfgtable.nocolor=true;
 if(!cfgtable.nocolor){
#ifndef NO_PTY
  if(!make_ptypair(0)||!make_ptypair(1))cfgtable.p.on=false;
  else cfgtable.p.on=true;
#endif
  if(pipe(fds)<0)cwexit(1,"pipe() failed.");
  if(pipe(fde)<0)cwexit(1,"pipe() failed.");
 }
#ifdef SIGCHLD
 sa.sa_handler=sighandler;
 sigemptyset(&sa.sa_mask);
 sa.sa_flags=SA_NOCLDSTOP;
 if(sigaction(SIGCHLD,&sa,0)<0){
#endif
  sig_catch(SIGTSTP,SIG_IGN);
#ifdef SIGCHLD
  sig_catch(SIGCHLD,sighandler);
 }
#endif
 sig_catch(SIGPIPE,sighandler);
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
#ifdef HAVE_SETSID
    setsid();
#endif
   }
   if(cfgtable.cmd)
    execle("/bin/sh",base_name(scrname),"-c",cfgtable.cmd,(char *)0,environ);
   else{
    argv[1]=base_scrname;
    execvpe(argv[1],&argv[1],environ);
   }
   /* shouldn't make it here. (no point to stay alive) */
   exit(1);
   break;
  default:
   /* parent process to read the program's output. (forwards SIGINT to child) */
   cfgtable.eint=true;
   sig_catch(SIGINT,sighandler);
#ifdef HAVE_SETPROCTITLE
#ifdef INT_SETPROCTITLE
   initsetproctitle(argc,argv,environ);
#endif
   setproctitle("cw: wrapping [%s] {pid=%u}",base_scrname,pid_c);
#endif
   buf=(char *)xzalloc(BUFSIZE+1);
#ifndef NO_PTY
   if(cfgtable.p.on){
    close(fds[0]);
    close(fde[0]);
    fds[0]=cfgtable.p.mout;
    fde[0]=cfgtable.p.merr;
   }
   fcntl(fds[0],F_SETFL,O_NONBLOCK);
   fcntl(fde[0],F_SETFL,O_NONBLOCK);
#endif
   fdm=((fds[0]>fde[0]?fds[0]:fde[0])+1);
   while(s>0||!ext){
    FD_ZERO(&rfds);
    FD_SET(fds[0],&rfds);
    FD_SET(fde[0],&rfds);
    if(select(fdm,&rfds,0,0,0)>=0){
     if(FD_ISSET(fds[0],&rfds))fd=fds[0];
     else if(FD_ISSET(fde[0],&rfds))fd=fde[0];
     else continue;
     memset(buf,0,BUFSIZE);
     while((s=read(fd,buf,BUFSIZE))>0){
      if(!on){
       j=0;
       tmp=(char *)xzalloc(s+1);
       on=true;
      }
      else
       tmp=(char *)xrealloc(tmp,s+j+1);
      for(i=0;s>i;i++){
       if(buf[i]==0x1b&&s>i+3&&buf[i+1]=='['){
        son=false;
        for(k=i+2;!son&&s>k;k++){
         if(buf[k]=='m'){
          if(k-i>2)i+=k-i;
          son=true;
         }
         else if(buf[k]!=';'&&!isdigit((unsigned char)buf[k]))son=true;
        }
       }
       else if(buf[i]=='\n'){
        tmp[j]=0;
        if(fd==fds[0])
          fprintf(stdout,"%s\n",cfgtable.nocolor_stdout?tmp:convert_string(tmp));
        else
          fprintf(stderr,"%s\n",cfgtable.nocolor_stderr?tmp:convert_string(tmp));
        fflush(fd==fds[0]?stdout:stderr);
        free(tmp);
        on=false;
        if(s>i){
         tmp=(char *)xzalloc(s-i+1);
         on=true;
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

/* handles each config file line. */
static void c_handler(char *line,size_t l,int argc){
 bool o=false,on=false;
 size_t i=0,j=0,k=0;
 char *tmp,*ptr;
 if(!strcmp(parameter(line," ",0),"ifos-else")){
  o=true;
  if(cfgtable.ifosa)cfgtable.ifos=(cfgtable.ifos?0:1);
  else c_error(l,"'ifos-else' used before any previous comparison.");
 }
 else if(!strcmp(parameter(line," ",0),"ifos")||!strcmp(pptr,"ifnos")){
  cfgtable.ifosa=o=true;
  if(!parameter(line," ",1))c_error(l,"'ifos'/'ifnos' syntax error. (not enough arguments?)");
  else{
   tmp=xstrdup(pptr);
   for(j=i=0;!j&&parameter(tmp,":",i);i++){
    if(!strcmp(pptr,"<any>"))j=1;
    else j=(struncmp(pptr)?0:1);
   }
   free(tmp);
   cfgtable.ifos=j;
   if(!strcmp(parameter(line," ",0),"ifos"))
    cfgtable.ifos=(cfgtable.ifos?0:1);
  }
 }
 else if(!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifarg-else"))){
  o=true;
  if(cfgtable.ifarga)cfgtable.ifarg=(cfgtable.ifarg?0:1);
  else c_error(l,"'ifarg-else' used before any previous comparison.");
 }
 else if(!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifarg")||
 !strcmp(pptr,"ifnarg"))){
  cfgtable.ifarga=o=true;
  if(!parameter(line," ",1))c_error(l,"'ifarg'/'ifnarg' syntax error. (not enough arguments?)");
  else{
   tmp=xstrdup(pptr);
   for(j=i=0;!j&&parameter(tmp,":",i);i++){
    if(!strcmp(pptr,"<any>")||(!strcmp(pptr,"<none>")&&argc<3))j=1;
    else j=!regxcmp(cfgtable.cmdargs,pptr);
   }
   free(tmp);
   cfgtable.ifarg=j;
   if(!strcmp(parameter(line," ",0),"ifarg"))
    cfgtable.ifarg=(cfgtable.ifarg?0:1);
  }
 }
 if(cfgtable.ifos||cfgtable.ifarg)
  return;
 if(line[0]=='#')return;
 else if(line[0]=='$'){
  j=strlen(line);
  for(i=0;j>i;i++)line[i]=line[i+1];
  line=strtok(line,"=");
  if(line&&strlen(line)){
   tmp=xstrdup(line);
   line=strtok(0,"");
   if(line&&strlen(line))
    setenv(tmp,line,1);
   else c_error(l,"environment variable value missing.");
   free(tmp);
  }
  else c_error(l,"environment variable name missing.");
 }
 else if(!strcmp(parameter(line," ",0),"command")){
  ptr=strtok(line," ");
  ptr=strtok(0,"");
  if((k=strlen(ptr))){
   if(cfgtable.cmd)free(cfgtable.cmd);
   cfgtable.cmd=(char *)xzalloc(k+strlen(cfgtable.cmdargs)+1);
   for(on=false,j=i=0;k>i;i++){
    if(!on&&!strncmp(ptr+i,"{}",2)){
     strcpy(cfgtable.cmd+j,cfgtable.cmdargs);
     j+=strlen(cfgtable.cmdargs);
     i++;
     on=true;
    }
    else{
     cfgtable.cmd[j]=ptr[i];
     j++;
    }
   }
   cfgtable.cmd[j]=0;
  }
  else c_error(l,"'command' instruction missing command.");
 }
 else if(!strcmp(parameter(line," ",0),"base")){
  if((cfgtable.base=color_atoi(parameter(line," ",1)))<0)
   c_error(l,"'base' definition used an invalid color.");
  if(cfgtable.base==16)cfgtable.base=7;
 }
 else if(!strcmp(parameter(line," ",0),"match")){
  if(parameter(line," ",1)){
   match *m=(match *)XZALLOC(match);
   tmp=xstrdup(pptr);
   if(color_atoi(parameter(tmp,":",0))>-1)
    m->b=color_atoi(parameter(tmp,":",0));
   else{
    c_error(l,"invalid first color in 'match' definition. (defaulting)");
    m->b=16;
   }
   if(color_atoi(parameter(tmp,":",1))>-1)
    m->a=color_atoi(parameter(tmp,":",1));
   else{
    c_error(l,"invalid second color in 'match' definition. (defaulting)");
    m->a=16;
   }
   free(tmp);
   if(parameter(line," ",2)){
    ptr=tmp=xstrdup(line);
    tmp=strtok(tmp," ");
    tmp=strtok(0," ");
    tmp=strtok(0,"");
    m->data=xstrdup(tmp);
    gl_list_add_last(cfgtable.m,m);
    free(ptr);
   }
   else c_error(l,"'match' syntax error: cannot find colors argument)");
  }
  else c_error(l,"'match' syntax error: cannot find regex argument");
 }
 else if(!o)c_error(l,"invalid definition instruction.");
}

/* reads the config file, passing the lines to c_handler(). */
void c_read(char *file,int argc){
 size_t i=0,l=0;
 char buf[BUFSIZE+1];
 FILE *fs;
 if(!(fs=fopen(file,"r")))cwexit(1,"failed opening config file.");
 while(fgets(buf,BUFSIZE,fs)){
  i=strlen(buf);
  if(i>2){
   /* remove excess characters read, ex. \r\n. */
   if(!isprint((unsigned char)buf[i-1]))buf[i-1]=0;
   if(!isprint((unsigned char)buf[i-2]))buf[i-2]=0;
   c_handler(buf,l++,argc);
  }
 }
 fclose(fs);
 if(cfgtable.base<0)cfgtable.base=7;
}

/* program start. */
int main(int argc,char **argv){
 int i=0,j=0;
 char *ptr,*newpath;
 set_program_name(argv[0]);
 scrname=xstrdup(argv[1]);
 base_scrname=base_name(scrname);
 cfgtable.z.l=cfgtable.z.h=-1;
 cfgtable.m=gl_list_create_empty(GL_LINKED_LIST,NULL,NULL,NULL,1);
 if(argc>1&&*argv[1]=='-'){
  for(i=1;i<argc;i++){
   if(!strcmp("--help",argv[i]))
    usage();
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
  }
 }
 for(i=2;argc>i;i++)j+=(strlen(argv[i])+1);
 cfgtable.cmdargs=(char *)xzalloc(j+1);
 j=0;
 for(i=2;argc>i;i++){
  sprintf(cfgtable.cmdargs+j,"%s%c",argv[i],(argc-i==1?0:32));
  j+=(strlen(argv[i])+1);
 }
 if(argc<=1)
  usage();
 if(access(argv[1],F_OK))
  cwexit(1,"cannot find definition file.");
 for(i=0;18>i;i++)
  pal2[i]=xstrdup(pal2_orig[i]);
 cfgtable.base=-1;
 cfgtable.ifarg=cfgtable.ifarga=false;
 cfgtable.ifos=cfgtable.ifosa=false;
#ifndef NO_PTY
 cfgtable.p.on=false;
#endif
 if(!cfgtable.z.on&&(ptr=getenv("CW_COLORIZE")))
  setcolorize(ptr);
 if(getenv("CW_INVERT"))cfgtable.invert=true;
 /* Set PATH for child processes; may be overridden by definition file. */
 newpath=remove_dir_from_path(getenv("PATH"),SCRIPTSDIR);
 setenv("PATH",newpath,1);
 free(newpath);
 cfgtable.nocolor+=getenv("NOCOLOR")?1:(getenv("MAKELEVEL")?1:0);
 c_read(scrname,argc);
 if(cfgtable.z.on)cfgtable.invert=false;
 cfgtable.nocolor_stdout=!isatty(STDOUT_FILENO);
 cfgtable.nocolor_stderr=!isatty(STDERR_FILENO);
 execcw(argc,argv);
 cwexit(0,0);
}
