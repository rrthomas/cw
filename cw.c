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
#include "progname.h"
#include "dirname.h"

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
void setcolorize(char *);
signed char execot(char *,unsigned char,unsigned int);
void execcw(signed int,char **);
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
 signed char nocolor_stdout;
 signed char nocolor_stderr;
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

static unsigned char ext=0,rexit=0;
static char *pal2[18],*aptr,*fptr,*pptr,*scrname;
static pid_t pid_p,pid_c;
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

static void *cwmalloc(size_t n) {
 void *p = calloc(1, n);
 if (!p) cwexit(1,"malloc() failed.");
 return p;
}

/* plucks a requested token out of a string. */
static char *parameter(const char *string,const char *delim,size_t p){
 size_t n=p;
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

/* filter a directory out of a PATH-like colon-separated path list. */
static char *remove_dir_from_path(const char *path, const char *dir){
 if(path){
  char *canon_dir=canonicalize_file_name(dir);
  size_t s=strlen(path),i=0;
  char *newpath=(char *)cwmalloc(s+1);
  char *tmp=(char *)cwmalloc(s+1);
  strcpy(tmp,path);
  while(strcmp(parameter(tmp,":",i++),"-1")){
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

/* program start. */
signed int main(signed int argc,char **argv){
 int i=0,j=0;
 char *ptr,*basename,*newpath;
 set_program_name(argv[0]);
 basename=base_name(program_name);
 cfgtable.z.l=cfgtable.z.h=-1;
 if(!strcmp(basename,"cw")){
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
 free(basename);
 for(i=2;argc>i;i++)j+=(strlen(argv[i])+1);
 cfgtable.cmdargs=(char *)cwmalloc(j+1);
 j=0;
 for(i=2;argc>i;i++){
  sprintf(cfgtable.cmdargs+j,"%s%c",argv[i],(argc-i==1?0:32));
  j+=(strlen(argv[i])+1);
 }
 if(argc>1){
  if(access(argv[1],F_OK))
   cwexit(1,"non-existent path to definition file.");
  scrname=(char *)cwmalloc(strlen(argv[1])+1);
  strcpy(scrname,argv[1]);
 }
 else
  usage();
 for(i=0;18>i;i++){
  pal2[i]=(char *)cwmalloc(strlen(pal2_orig[i])+1);
  strcpy(pal2[i],pal2_orig[i]);
 }
 if(getenv("CW_CHK_SETCODE"))
  cfgtable.ec=execot(getenv("CW_CHK_SETCODE"),2,0);
 cfgtable.base=-1;
 cfgtable.ifarg=cfgtable.ifarga=0;
 cfgtable.ifos=cfgtable.ifosa=cfgtable.ifexit=cfgtable.ifexita=0;
#ifndef NO_PTY
 cfgtable.p.on=0;
#endif
 if(!cfgtable.z.on&&(ptr=(char *)getenv("CW_COLORIZE")))
  setcolorize(ptr);
 if(getenv("CW_INVERT"))cfgtable.invert=1;
 /* Set PATH for child processes; may be overridden by configuration file. */
 newpath=remove_dir_from_path(getenv("PATH"),SCRIPTSDIR);
 setenv("PATH",newpath,1);
 free(newpath);
 c_read(scrname,argc);
 cfgtable.nocolor+=(getenv("NOCOLOR")?1:0);
 cfgtable.nocolor+=(getenv("MAKELEVEL")?1:0); /* FIXME: document this */
 if(!cfgtable.nocolor&&getenv("CW_CHK_NOCOLOR"))
  cfgtable.nocolor=(execot(getenv("CW_CHK_NOCOLOR"),2,0)?1:0);
 if(getenv("NOCOLOR_NEXT")){
  setenv("NOCOLOR","1",1);
  unsetenv("NOCOLOR_NEXT");
 }
 if(cfgtable.z.on)cfgtable.invert=0;
 if(cfgtable.fc&&cfgtable.nocolor)cfgtable.nocolor=0;
 cfgtable.nocolor_stdout=!isatty(STDOUT_FILENO);
 cfgtable.nocolor_stderr=!isatty(STDERR_FILENO);
 execcw(argc,argv);
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
/* converts the original string to a color string based on the config file. */
static char *convert_string(const char *line){
 unsigned char on=0;
 unsigned int i=0,j=0,k=0,l=0;
 char *buf,*tbuf,*tmp,*tmpcmp;
 regex_t re;
 regmatch_t pm;
 size_t s=strlen(line);
 tbuf=(char *)cwmalloc(s+1);
 strcpy(tbuf,line);
 /* start processing the 'match' definitions. */
 if(cfgtable.m.tot){
  for(j=i=0;i<cfgtable.m.tot;i++){
   s=strlen(tbuf);
   tmp=(char *)cwmalloc(s*(cfgtable.m.tot*16+1)+s+1);
   on=j=l=k=0;
   if(regcomp(&re,cfgtable.m.data[i],REG_EXTENDED))
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
     &&cfgtable.m.b[i]!=17){
      strcpy(tmp+j,pal2[cfgtable.m.b[i]==16?cfgtable.base:cfgtable.m.b[i]]);
      j+=strlen(pal2[cfgtable.m.b[i]==16?cfgtable.base:cfgtable.m.b[i]]);
     }
     strcpy(tmp+j,tmpcmp);
     j+=strlen(tmpcmp);
     free(tmpcmp);
     if(!on&&!memchr(tbuf+(k-(k<7?k:7)),'\x1b',(k<7?k:7))
     &&cfgtable.m.a[i]!=17){
      strcpy(tmp+j,pal2[cfgtable.m.a[i]==16?cfgtable.base:cfgtable.m.a[i]]);
      j+=strlen(pal2[cfgtable.m.a[i]==16?cfgtable.base:cfgtable.m.a[i]]);
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
                      +cfgtable.b.llen+cfgtable.b.rlen+4+1);
 sprintf(buf,"%s%s%s%s%s",(cfgtable.b.llen?cfgtable.b.ldata:"")
 ,pal2[cfgtable.base],tbuf,(cfgtable.b.rlen?cfgtable.b.rdata:""),
 pal2[16]);
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
   if(type<2)c_error(l,"'!'/'@' failed to execute background program.");
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
noreturn void execcw(signed int argc,char **argv){
 unsigned char on=0,son=0;
 int i=0,j=0,k=0;
 signed char re=0;
 signed int fds[2],fde[2],fdm=0,fd=0,s=0,e=0;
 char *buf,*tmp;
 fd_set rfds;
#ifdef SIGCHLD
 struct sigaction sa;
#endif
 if(!(cfgtable.m.tot))
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
    argv[1]=strpname(scrname);
    execvpe(base_name(scrname),&argv[1],environ);
   }
   /* shouldn't make it here. (no point to stay alive) */
   exit(1);
   break;
  default:
   /* parent process to read the program's output. (forwards SIGINT to child) */
   cfgtable.eint=1;
   signal(SIGINT,sighandler);
#ifdef HAVE_SETPROCTITLE
#ifdef INT_SETPROCTITLE
   initsetproctitle(argc,argv,environ);
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
       if(buf[i]==0x1b&&s>i+3&&buf[i+1]=='['){
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
        if(fd==fds[0])
          fprintf(stdout,"%s\n",cfgtable.nocolor_stdout?tmp:convert_string(tmp));
        else
          fprintf(stderr,"%s\n",cfgtable.nocolor_stderr?tmp:convert_string(tmp));
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
 unsigned int i=0,j=0,k=0;
 char *tmp,*ptr;
 if(!strcmp(parameter(line," ",0),"ifos-else")){
  o=1;
  if(cfgtable.ifosa)cfgtable.ifos=(cfgtable.ifos?0:1);
  else c_error(l,"'ifos-else' used before any previous comparison.");
 }
 else if(!strcmp(parameter(line," ",0),"ifos")||!strcmp(pptr,"ifnos")){
  cfgtable.ifosa=o=1;
  if(!strcmp(parameter(line," ",1),"-1"))c_error(l,"'ifos'/'ifnos' syntax error. (not enough arguments?)");
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
  else c_error(l,"'ifexit-else' used before any previous comparison.");
 }
 else if(!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifexit")||!strcmp(pptr,"ifnexit"))){
  cfgtable.ifexita=o=1;
  if(atoi(parameter(line," ",1))>127||atoi(pptr)<-127)
   c_error(l,"'ifexit'/'ifnexit' invalid exit level. (-127..127)");
  else{
   if(!strcmp(pptr,"<any>")||atoi(pptr)==cfgtable.ec)cfgtable.ifexit=1;
   if(!strcmp(parameter(line," ",0),"ifexit"))
    cfgtable.ifexit=(cfgtable.ifexit?0:1);
  }
 }
 else if(!cfgtable.ifexit&&!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifarg-else"))){
  o=1;
  if(cfgtable.ifarga)cfgtable.ifarg=(cfgtable.ifarg?0:1);
  else c_error(l,"'ifarg-else' used before any previous comparison.");
 }
 else if(!cfgtable.ifexit&&!cfgtable.ifos&&(!strcmp(parameter(line," ",0),"ifarg")||
 !strcmp(pptr,"ifnarg"))){
  cfgtable.ifarga=o=1;
  if(!strcmp(parameter(line," ",1),"-1"))c_error(l,"'ifarg'/'ifnarg' syntax error. (not enough arguments?)");
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
   else c_error(l,"environment variable value missing.");
   free(tmp);
  }
  else c_error(l,"environment variable name missing.");
 }
 else if(line[0]=='!'||line[0]=='@'){
  if(strlen(line)>1)
   cfgtable.ec=execot(line+1,(line[0]=='!'?0:1),l);
 }
 else if(!strcmp(parameter(line," ",0),"command")){
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
  else c_error(l,"'command' instruction missing command.");
 }
 else if(!strcmp(parameter(line," ",0),"base")){
  if((cfgtable.base=color_atoi(parameter(line," ",1)))<0)
   c_error(l,"'base' definition used an invalid color.");
  if(cfgtable.base==16)cfgtable.base=7;
 }
 else if(!strcmp(parameter(line," ",0),"match")){
  if(strcmp(parameter(line," ",1),"-1")){
   tmp=(char *)cwmalloc(strlen(pptr)+1);
   strcpy(tmp,pptr);
   if(color_atoi(parameter(tmp,":",0))>-1)
    cfgtable.m.b[cfgtable.m.cur]=color_atoi(parameter(tmp,":",0));
   else{
    c_error(l,"invalid first color in 'match' definition. (defaulting)");
    cfgtable.m.b[cfgtable.m.cur]=16;
   }
   if(color_atoi(parameter(tmp,":",1))>-1)
    cfgtable.m.a[cfgtable.m.cur]=color_atoi(parameter(tmp,":",1));
   else{
    c_error(l,"invalid second color in 'match' definition. (defaulting)");
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
    if(cfgtable.m.cur>cfgtable.m.tot)
     c_error(l,"'match' too many entries. (race condition?)");
    else{
     cfgtable.m.data[cfgtable.m.cur]=(char *)cwmalloc(strlen(tmp)+1);
     strcpy(cfgtable.m.data[cfgtable.m.cur],tmp);
     cfgtable.m.cur++;
    }
    free(ptr);
   }
   else c_error(l,"'match' syntax error: cannot find colors argument)");
  }
  else c_error(l,"'match' syntax error: cannot find regex argument");
 }
 else if(!strcmp(parameter(line," ",0),"nocolor"))cfgtable.nocolor=1;
 else if(!strcmp(parameter(line," ",0),"forcecolor"))cfgtable.fc=1;
 else if(!o)c_error(l,"invalid definition instruction.");
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
  }
  while(fgets(buf,BUFSIZE,fs)){
   /* find the amount of definitions to store in memory. */
   if(!i){
    if(!strcmp(parameter(buf," ",0),"match"))cfgtable.m.tot++;
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
}
/* configuration error message. */
void c_error(unsigned int l,const char *text){
 fprintf(stdout,"cw:definition_error:%u: %s\n",l,text);
}
/* exit with or without a reason, resets color too. */
noreturn void cwexit(signed char level,const char *reason){
 if(!rexit&&level)fprintf(stdout,"cw:exit: %s\n",reason);
 fflush(stdout);
 exit(level);
}
