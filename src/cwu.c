/* cwu -- color wrapper directive updater for cw(color wrapper).
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glob.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "cw.h"

void globcw(char *,char *);
void modify_directive(char *,char *,mode_t);
void c_notice(char *,char *);
void cwuexit(signed char,char *);

char id[]="$Id: cwu.c,v "VERSION" "CMPTIME" v9/fakehalo Exp $";

/* program start */
signed int main(signed int argc,char **argv){
 if(argc>1&&!strcmp(argv[1],"-v"))
  cwuexit(1,"cwu (color wrapper directive updater) v"VERSION);
 if(argc<2)
  cwuexit(1,"syntax: cwu <definitions directory> <cw binary path>");
 if(argc<3)cwuexit(1,"no path provided to update with.");
 if(strncmp(argv[2]+(strlen(argv[2])-3),"/cw",3))
  cwuexit(1,"cw binary path doesn't end with `/cw'.");
 if(access(argv[2],X_OK))
  cwuexit(1,"cw binary does not exist or is not executable.");
 globcw(argv[1],argv[2]);
 cwuexit(0,0);
 exit(0);
}
/* obtain the list of files to update in a directory. */
void globcw(char *path,char *directive){
 unsigned int i=0;
 char *gpath;
 struct stat mod;
 glob_t gb;
 gb.gl_offs=1;
 if(path[strlen(path)-1]=='/')path[strlen(path)-1]=0;
 if(!(gpath=(char *)malloc(strlen(path)+2+1)))
  cwuexit(1,"malloc() failed.");
 sprintf(gpath,"%s/*",path);
 glob(gpath,0,0,&gb);
 free(gpath);
 for(i=0;gb.gl_pathc>i;i++){
  if(stat(gb.gl_pathv[i],&mod))c_notice(gb.gl_pathv[i],"stat() failed.");
  else if(S_ISDIR(mod.st_mode))c_notice(gb.gl_pathv[i],"is a directory.");
  else if(getuid()&&mod.st_uid!=getuid())
   c_notice(gb.gl_pathv[i],"file is not owned by the current uid.");
  else modify_directive(gb.gl_pathv[i],directive,mod.st_mode);
 }
 globfree(&gb);
 return;
}
/* do the actual directive change. */
void modify_directive(char *file,char *directive,mode_t mode){
 unsigned char i=0;
 unsigned int s=0;
 char buf[BUFSIZE+1],tmpfile[4096+1];
 FILE *fsr,*fsw;
 if(!(fsr=fopen(file,"r"))){
  c_notice(file,"could not open file.");
  return;
 }
 if(strlen(file)>4092){
  c_notice(file,"path too long to create.");
  fclose(fsr);
  return;
 }
 sprintf(tmpfile,"%s.cwu",file);
 for(memset(buf,0,BUFSIZE);fgets(buf,BUFSIZE,fsr);memset(buf,0,BUFSIZE)){
  s=strlen(buf);
  if(buf[s]=='\n')s--;
  if(buf[s]=='\r')s--;
  if(!i){
   if(s>4&&!strncmp(buf,"#!",2)&&!strncmp(buf+(s-4),"/cw",3)){
    i=1;
    if(!access(tmpfile,F_OK)){
     c_notice(tmpfile,"temporary filename in use.");
     fclose(fsr);
     return;
    }
    if(!(fsw=fopen(tmpfile,"w"))){
     c_notice(tmpfile,"could not write to temporary file.");
     fclose(fsr);
     return;
    }
    fprintf(fsw,"#!%s\n",directive);
   }
   else{
    c_notice(file,"doesn't appear to be a valid cw definition file.");
    fclose(fsr);
    return;
   }
  }
  else fputs(buf,fsw);
 }
 fclose(fsr);
 fclose(fsw);
 if(!i)c_notice(file,"file contained no entry? (unchanged)");
 else{
  if(rename(tmpfile,file))
   c_notice(file,"rename() failed. (DIRECTIVE CHANGE FAILED)");
  else{
   c_notice(file,"definition file updated.");
  }
  chmod(file,mode);
 }
 return;
}
/* report an error to modify file, but do not exit. */
void c_notice(char *fname,char *text){
 fprintf(stdout,"cwu:notice: `%s' %s\n",fname,text);
 return;
}
/* exit with or without a reason. */
void cwuexit(signed char level,char *reason){
 if(level)fprintf(stdout,"cwu:exit: %s\n",reason);
 exit(level);
}
