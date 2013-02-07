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
#include "hash.h"
#include "xalloc.h"

#define BUFSIZE 1024

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

/* Initialize pseudo setproctitle. */
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

/* Pseudo setproctitle. */
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

/* Match instruction. */
typedef struct{
  char *data;
  unsigned char b;
  unsigned char a;
}match;

/* Colormap hash entry. */
typedef struct{
 const char *log;
 const char *phys;
}colormap_t;

char id[]="$Id: cw.c,v "VERSION" v9/fakehalo Exp $";

static bool ext=false;
static unsigned char rexit=0;
static char *pal2[18],*scrname,*base_scrname;
static Hash_table *colormap;
static pid_t pid_c;
extern char **environ;
static bool ifarg, ifarga;
static bool ifos, ifosa;
static bool eint;
static bool invert;
static bool nocolor, nocolor_stdout, nocolor_stderr;
static char *cmd, *cmdargs;
static gl_list_t matches;
static int master[2];
static int slave[2];
static bool ptys_on;

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

/* Definition error message. */
void c_error(size_t l,const char *text){
 fprintf(stdout,"cw:definition error:%zu: %s\n",l,text);
}

/* Exit with or without a reason. */
noreturn void cwexit(signed char level,const char *reason){
 if(!rexit&&level)fprintf(stdout,"cw:exit: %s\n",reason);
 fflush(stdout);
 exit(level);
}

void xalloc_die(void) {
 cwexit(1,"malloc() failed.");
}

/* Check for a regex match of a string. */
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

/* Convert a logical color string to a physical color array index. (0-17) */
static _GL_ATTRIBUTE_PURE signed char color_atoi(const char *color){
 if(color){
  colormap_t *c=XZALLOC(colormap_t),*ent;
  signed char i=0;
  const char **palptr=invert?pal1_invert:pal1;
  c->log=color;
  ent=hash_lookup(colormap,c);
  if(!ent){ /* Use "default" if the color type is undefined. */
   c->log="default";
   ent=hash_lookup(colormap,c);
  }
  free(c);
  if(ent){
   while(strcmp(palptr[i],ent->phys)&&i<18)i++;
   return(i<18?i:-1);
  }
 }
 return(-1);
}

const char *default_colormap="default=cyan:bright=cyan+:highlight=green+:lowlight=green:neutral=white:warning=yellow:error=red+:none=none:punctuation=blue+";

static size_t colormap_hash (const void *c, size_t n){
 return hash_string(((const colormap_t *)c)->log,n);
}

static bool colormap_cmp (const void *c1, const void *c2){
 return !strcmp(((const colormap_t *)c1)->log, ((const colormap_t *)c2)->log);
}

/* Set user color map. */
static void setcolors(const char *str){
 char *ass,*tmp=xstrdup(str);
 size_t i;
 for(i=0;(ass=parameter(tmp,":",i));i++){
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
 if(color_atoi("default")==-1)
  setcolors("default=white");
}

/* Color a string based on the definition file. */
static char *convert_string(const char *line){
 gl_list_iterator_t i;
 size_t j=0,k=0,l=0;
 const match *m;
 char *buf;
 regex_t re;
 regmatch_t pm;
 char *tbuf=xstrdup(line);
 /* Process the 'match' definitions. */
 for(j=0,i=gl_list_iterator(matches);gl_list_iterator_next(&i,(const void **)&m,NULL);){
  size_t s=strlen(tbuf);
  bool on=false;
  j=l=k=0;
  if(!regcomp(&re,m->data,REG_EXTENDED)){
   char *tmpcmp,*tmp=(char *)xzalloc(s*(gl_list_size(matches)*16+1)+s+1);
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
     strcpy(tmp+j,pal2[m->b==16?color_atoi("default"):m->b]);
     j+=strlen(pal2[m->b==16?color_atoi("default"):m->b]);
    }
    strcpy(tmp+j,tmpcmp);
    j+=strlen(tmpcmp);
    free(tmpcmp);
    if(!on&&!memchr(tbuf+(k-(k<7?k:7)),'\x1b',(k<7?k:7))&&m->a!=17){
     strcpy(tmp+j,pal2[m->a==16?color_atoi("default"):m->a]);
     j+=strlen(pal2[m->a==16?color_atoi("default"):m->a]);
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
 buf=(char *)xzalloc(strlen(pal2[color_atoi("default")])+strlen(tbuf)+4+1);
 sprintf(buf,"%s%s%s",pal2[color_atoi("default")],tbuf,pal2[16]);
 free(tbuf);
 return(buf);
}

/* Create a master-slave pty pair. */
static bool make_ptypair(unsigned char v){
#ifdef HAVE_OPENPTY
 return openpty(&master[v],&slave[v],0,0,0)==0;
#else
 return false;
#endif
}

static void sighandler(int sig){
 if(sig==SIGINT&&eint){
  if(pid_c){
   kill(pid_c,SIGINT);
   eint=false;
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


/* Execute and color a program. */
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
 if(!matches)
  nocolor=true;
 if(!nocolor){
  ptys_on=make_ptypair(0)&&make_ptypair(1);
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
 switch(nocolor?0:(pid_c=fork())){
  case -1:
   cwexit(1,"fork() error.");
   break;
  case 0:
   /* child process to execute the program. */
   if(!nocolor){
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
   }
   if(cmd)
    execle("/bin/sh",base_name(scrname),"-c",cmd,(char *)0,environ);
   argv[1]=base_scrname;
   execvpe(argv[1],&argv[1],environ);
   abort(); /* We never get here. */
  default:
   /* parent process to read the program's output. (forwards SIGINT to child) */
   eint=true;
   sig_catch(SIGINT,sighandler);
#ifdef HAVE_SETPROCTITLE
#ifdef INT_SETPROCTITLE
   initsetproctitle(argc,argv,environ);
#endif
   setproctitle("cw: wrapping [%s] {pid=%u}",base_scrname,pid_c);
#endif
   buf=(char *)xzalloc(BUFSIZE+1);
   if(ptys_on){
    close(fds[0]);
    close(fde[0]);
    fds[0]=master[0];
    fde[0]=master[1];
   }
   fcntl(fds[0],F_SETFL,O_NONBLOCK);
   fcntl(fde[0],F_SETFL,O_NONBLOCK);
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
        char *aptr=NULL;
        tmp[j]=0;
        if(fd==fds[0])
         fprintf(stdout,"%s\n",nocolor_stdout?tmp:(aptr=convert_string(tmp)));
        else
         fprintf(stderr,"%s\n",nocolor_stderr?tmp:(aptr=convert_string(tmp)));
        free(aptr);
        fflush(fd==fds[0]?stdout:stderr);
        free(tmp);
        on=false;
        if(s>i){
         tmp=(char *)xzalloc(s-i+1);
         on=true;
        }
        j=0;
       }
       else if(buf[i]!='\r')tmp[j++]=buf[i];
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

/* Process a definition file line. */
static void c_handler(char *line,size_t l,int argc){
 bool o=false,on=false;
 size_t i=0,j=0,k=0;
 char *tmp,*ptr,*ins=parameter(line," ",0);
 if(!strcmp(ins,"ifos-else")){
  o=true;
  if(ifosa)ifos=(ifos?0:1);
  else c_error(l,"'ifos-else' used before any previous comparison.");
 }
 else if(!strcmp(ins,"ifos")||!strcmp(ins,"ifnos")){
  ifosa=o=true;
  tmp=parameter(line," ",1);
  if(!tmp)c_error(l,"'ifos'/'ifnos' syntax error. (not enough arguments?)");
  else{
   for(j=i=0;!j&&(ptr=parameter(tmp,":",i));i++){
    if(!strcmp(ptr,"<any>"))j=1;
    else j=(struncmp(ptr)?0:1);
    free(ptr);
   }
   free(tmp);
   ifos=j;
   if(!strcmp(ins,"ifos"))
    ifos=(ifos?0:1);
  }
 }
 else if(!ifos&&(!strcmp(ins,"ifarg-else"))){
  o=true;
  if(ifarga)ifarg=(ifarg?0:1);
  else c_error(l,"'ifarg-else' used before any previous comparison.");
 }
 else if(!ifos&&(!strcmp(ins,"ifarg")||!strcmp(ins,"ifnarg"))){
  ifarga=o=true;
  tmp=parameter(line," ",1);
  if(!tmp)c_error(l,"'ifarg'/'ifnarg' syntax error. (not enough arguments?)");
  else{
   for(j=i=0;!j&&(ptr=parameter(tmp,":",i));i++){
    if(!strcmp(ptr,"<any>")||(!strcmp(ptr,"<none>")&&argc<3))j=1;
    else j=!regxcmp(cmdargs,ptr);
    free(ptr);
   }
   free(tmp);
   ifarg=j;
   if(!strcmp(ins,"ifarg"))
    ifarg=(ifarg?0:1);
  }
 }
 if(ifos||ifarg)
  return;
 if(line[0]=='#')return;
 else if(line[0]=='$'){
  line=strtok(line+1,"=");
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
 else if(!strcmp(ins,"command")){
  ptr=strtok(line," ");
  ptr=strtok(0,"");
  if((k=strlen(ptr))){
   if(cmd)free(cmd);
   cmd=(char *)xzalloc(k+strlen(cmdargs)+1);
   for(on=false,j=i=0;k>i;i++){
    if(!on&&!strncmp(ptr+i,"{}",2)){
     strcpy(cmd+j,cmdargs);
     j+=strlen(cmdargs);
     i++;
     on=true;
    }
    else{
     cmd[j]=ptr[i];
     j++;
    }
   }
   cmd[j]=0;
  }
  else c_error(l,"'command' instruction missing command.");
 }
 else if(!strcmp(ins,"match")){
  if((tmp=parameter(line," ",1))){
   match *m=(match *)XZALLOC(match);
   if(color_atoi((ptr=parameter(tmp,":",0)))>-1){
    m->b=color_atoi(ptr);
    free(ptr);
   }
   else{
    c_error(l,"invalid first color in 'match' definition. (defaulting)");
    m->b=16;
   }
   if(color_atoi((ptr=parameter(tmp,":",1)))>-1){
    m->a=color_atoi(ptr);
    free(ptr);
   }
   else{
    c_error(l,"invalid second color in 'match' definition. (defaulting)");
    m->a=16;
   }
   free(tmp);
   if((ptr=parameter(line," ",2))){
    free(ptr);
    ptr=tmp=xstrdup(line);
    tmp=strtok(tmp," ");
    tmp=strtok(0," ");
    tmp=strtok(0,"");
    m->data=xstrdup(tmp);
    gl_list_add_last(matches,m);
    free(ptr);
   }
   else c_error(l,"'match' syntax error: cannot find colors argument)");
  }
  else c_error(l,"'match' syntax error: cannot find regex argument");
 }
 else if(!o)c_error(l,"invalid definition instruction.");
 free(ins);
}

/* Read the config file, passing the lines to c_handler(). */
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
}

int main(int argc,char **argv){
 int i=0;
 size_t j=0;
 char *ptr,*newpath;
 set_program_name(argv[0]);
 if(argc>1)scrname=xstrdup(argv[1]);
 base_scrname=base_name(scrname?scrname:program_name);
 matches=gl_list_create_empty(GL_LINKED_LIST,NULL,NULL,NULL,1);
 if(argc>1&&*argv[1]=='-'){
  for(i=1;i<argc;i++){
   if(!strcmp("--help",argv[i]))
    usage();
   else if(!strcmp("--version",argv[i])){
    cwexit(1,"cw (color wrapper) v"VERSION" (features="
#ifdef HAVE_OPENPTY
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
 cmdargs=(char *)xzalloc(j+1);
 j=0;
 for(i=2;argc>i;i++){
  sprintf(cmdargs+j,"%s%c",argv[i],(argc-i==1?0:32));
  j+=(strlen(argv[i])+1);
 }
 if(argc<=1)
  usage();
 if(access(argv[1],F_OK))
  cwexit(1,"cannot find definition file.");
 for(i=0;18>i;i++)
  pal2[i]=xstrdup(pal2_orig[i]);
 ifarg=ifarga=false;
 ifos=ifosa=false;
 ptys_on=false;
 colormap=hash_initialize(16,NULL,colormap_hash,colormap_cmp,NULL);
 if((ptr=getenv("CW_COLORS")))
  setcolors(ptr);
 else setcolors(default_colormap);
 if(getenv("CW_INVERT"))invert=true;
 /* Set PATH for child processes; may be overridden by definition file. */
 newpath=remove_dir_from_path(getenv("PATH"),SCRIPTSDIR);
 setenv("PATH",newpath,1);
 free(newpath);
 if(getenv("NOCOLOR"))nocolor=true;
 c_read(scrname,argc);
 nocolor_stdout=!isatty(STDOUT_FILENO);
 nocolor_stderr=!isatty(STDERR_FILENO);
 execcw(argc,argv);
}
