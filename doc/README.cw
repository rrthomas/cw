NAME
       cw - color wrapper for common unix commands

SYNOPSIS
       cw [-v|-V]

DESCRIPTION
       cw  is a free color wrapper designed to be a color gateway between pro-
       grams and the output they produce.  The colors applied are based on the
       definition  instructions  in  a  cw  executable file.  These definition
       files appear under the same name as the  program  they  are  executing,
       except  they  are  located  in NONE/lib/cw/.  The cw executable file in
       NONE/lib/cw/ will have a path definition instruction in it to point  it
       to the real program.

       If  a  cw executable file has an '@' on the end of the filename cw will
       assume the file is text, instead of a program, and will  use  the  path
       definition instruction as a path to the text file.

       The path (or other) definition instruction is the only required defini-
       tion in a cw executable file.


       path [path:path:...]
              path list separated by colons  to  the  program  being  wrapped.
              <env>  can  be  inserted  to use the PATH environmental variable
              (traditional $PATH-style format)

       other [program]
              uses specific program(s), in shell execution format, instead  of
              using path to find and execute the program.  "{}" can be used in
              the program string to insert the command-line  arguments   (path
              and other can not be used with eachother)

       ifarg [text:text:...]
              list  separated  by  colons  which  will be compared against the
              arguments called to the program.  if the text matches the  argu-
              ments,  then  the  definitions to follow will be used--otherwise
              they will not.  this will continue until the next ifarg,  ifnarg
              or ifarg-else definition instruction is used.  regex comparisons
              can be made instead of standard string comparisons by using  the
              useifregex definition instruction ("<any>" can be used as a con-
              stant value for any value, including no value.  "<none>" can  be
              used as a constant value for no command-line arguments)

       ifnarg [text:text:...]
              reverse of ifarg

       ifarg-else
              this will use the definitions to follow if the previous ifarg or
              ifnarg was false

       ifexit [value]
              compares the value to the last returned exit code from the ! and
              @ definition instructions.  if the value matches, then the defi-
              nitions to follow will be used--otherwise they will  not.   this
              will continue until the next ifexit, ifnexit or ifexit-else def-
              inition instruction is used.   this  definition  instruction  is
              intended  to create a gateway to the system to check for special
              scenarios in the form  of  user-made  scripts/programs.   ifexit
              definition instructions are a higher priority than ifarg.  value
              must be a numerical value between -127 and 127,  if  nothing  is
              placed  in value it will default to -1 ("<any>" can be used as a
              constant value for any value.  "<pseudo>" can be used as a  con-
              stant  value  for non-existent support of ifexit and ifnexit, it
              will only match if not supported) {THIS MAY NOT BE INCLUDED UPON
              COMPILE, RUN cw -v TO SEE IF "w" IS INCLUDED IN THE SUPPORT}

       ifnexit [value]
              reverse of ifexit {THIS MAY NOT BE INCLUDED UPON COMPILE, RUN cw
              -v TO SEE IF "w" IS INCLUDED IN THE SUPPORT}

       ifexit-else
              this will use the definitions to follow if the  previous  ifexit
              or ifnexit was false {THIS MAY NOT BE INCLUDED UPON COMPILE, RUN
              cw -v TO SEE IF "w" IS INCLUDED IN THE SUPPORT}

       ifos [text:text:...]
              list separated by colons which  will  be  compared  against  the
              operating  system  name(equal to uname -s).  if the text matches
              the operating system name, then the definitions to  follow  will
              be  used--otherwise they will not.  this will continue until the
              next ifos, ifnos or ifos-else definition  instruction  is  used.
              ifos  definition  instructions  are a higher priority than ifarg
              and ifexit.  regex comparisons can be made instead  of  standard
              string  comparisons  by using the useifregex definition instruc-
              tion ("<any>" can be used as a constant  value  for  any  value,
              including  no value.  "<pseudo>" can be used as a constant value
              for non-existent support of ifos and ifnos, it will  only  match
              if not supported) {THIS MAY NOT BE INCLUDED UPON COMPILE, RUN cw
              -v TO SEE IF "o" IS INCLUDED IN THE SUPPORT}

       ifnos [text:text:...]
              reverse of ifos {THIS MAY NOT BE INCLUDED UPON COMPILE,  RUN  cw
              -v TO SEE IF "o" IS INCLUDED IN THE SUPPORT}

       ifos-else
              this  will use the definitions to follow if the previous ifos or
              ifnos was false {THIS MAY NOT BE INCLUDED UPON COMPILE,  RUN  cw
              -v TO SEE IF "o" IS INCLUDED IN THE SUPPORT}

       base [color]
              color  used  if no other definitions state otherwise (if this is
              the only definition used it will be ignored)

       match [before color:after color] [match text]
              changes the color before and after the match text, if the  match
              text matched output from the program.  {?} is a single character
              wildcard that can be placed in match text.  the regex definition
              instruction is recommended for more complex matching

       regex [before color:after color] [regex pattern]
              changes  the  color  before  and after the regex pattern, if the
              regex pattern matched output from the program.   be  aware  that
              regex matching is the last definition instruction to be compared
              against output text, as such it can have an  adverse  effect  on
              the  previous  (coloring) definition instructions.  it is recom-
              mended to use the match definition instruction if pattern match-
              ing  is not necessary, this is  because (cpu) processing is much
              faster with the match definition instruction  (see  the  regex.7
              manual  page, which should already be on your system, for match-
              ing guidelines) {THIS MAY NOT BE INCLUDED UPON COMPILE,  RUN  cw
              -v TO SEE IF "r" IS INCLUDED IN THE SUPPORT}

       token [before color:after color] [slot value] [delim value]
              changes  the  color before and after the slot value based on the
              delim value.  the delim value is equal to a single ASCII charac-
              ter  in the form of 1-255 which is used as a start/stop point to
              assign each slot value (32, a space, is the most  commonly  used
              delim value)

       digit [before color:after color]
              changes the color before and after any number (0-9)

       ucase [before color:after color]
              changes the color before and after any uppercase letter (A-Z)

       lcase [before color:after color]
              changes the color before and after any lowercase letter (a-z)

       remap [color] [##;## | color]
              remaps   the   internal   color   value   to   the   ANSI  value
              "^[[##;##m"(where # is a single digit and only the ##;##  should
              be supplied) or color(original/un-remapped).  the colors random,
              random+, random&, default and none may not be remapped  or  used
              with  this  definition instruction (ex. "remap grey 30;47" would
              change the grey color to a grey highlighted effect)

       supermap [mapname]
              changes the internal color format to an entirely different color
              format(ie.  non-ANSI),  this is intended for special non-console
              coloring situations.  supermaps are internal and can  be  listed
              by  running cw -V.  if supermap and remap are both defined, then
              supermap will be forcefully disabled due to  potential  internal
              conflicts

       proctitle [title]
              changes  the  process  title to the provided title, this is will
              change how the program appears in programs like ps and  w  {THIS
              MAY  NOT  BE  INCLUDED  UPON COMPILE, RUN cw -v TO SEE IF "s" IS
              INCLUDED IN THE SUPPORT}

       print [string]
              prints the string to standard output as it is placed in the def-
              inition file.  a linefeed is not automatically placed on the end
              of the string, it is expected for the user to handle this.   \\,
              \e,  \r,  \n, \t, \v, \x??, \C[color] are supported conversions.
              if color conversions(\C) are used in the string, then the  print
              definition  instruction  should  be  placed  after  the supermap
              and/or random definition instruction, if they are present in the
              definition file (\x where available)

       header [file]
              displays the contents of file before any output of the real pro-
              gram

       footer [file]
              displays the contents of file after the last output of the  real
              program

       lborder [string]
              prepends the string to each (colored) line, this is intended for
              special processing outside of cw.  the string is not colored

       rborder [string]
              appends the string to each (colored) line, this is intended  for
              special processing outside of cw.  the string is not colored

       limit [number]
              only  displays  number of characters per line. ("columns" to use
              the environmental variable $COLUMNS instead of a static value)

       wait [secs] [usecs]
              number of extra secs and usecs to wait for output after the real
              program has exited.  this definition is rarely necessary

       random [color:color:...]
              creates  a  new  random colorset based on a list of colors sepa-
              rated by colons.  the random color selected will be used for the
              colors random, random+ and random& in definition instructions to
              come (color values explicitly used in context to the random def-
              inition  instruction are: black, blue, green, cyan, red, purple,
              brown, grey, grey+)

       random-goto [label:label:...]
              jumps to a random label from a specified list of  labels.   this
              is used when the random definition instruction is too limited of
              an environment to create a decent  randomized  definition  file.
              labels  bypass  ifexit, ifos, and ifarg restrictions (labels are
              specified by a colon and placed throughout the definition  file.
              ie.  ":label1"  alone on a line followed by any desired instruc-
              tions, the instructions to follow will be  processed  until  the
              next label)

       usepty allocates a pty to emulate a programs natural environment.  this
              can be useful for programs that expect a terminal  to  be  used,
              this  may  include  programs  that  appear to hang and dump text
              either when the program exits or in large chunks while  running.
              {THIS  MAY NOT BE INCLUDED UPON COMPILE, RUN cw -v TO SEE IF "p"
              IS INCLUDED IN THE SUPPORT}

       useifregex
              enables use of regex comparisons instead of standard string com-
              parisons  for  the  ifarg and ifos definition instructions {THIS
              MAY NOT BE INCLUDED UPON COMPILE, RUN cw -v TO  SEE  IF  "r"  IS
              INCLUDED IN THE SUPPORT}

       clear  clears the screen before the program is started

       forcecolor
              forces  color wrapping no matter what anti-coloring measures are
              enabled

       nocolor
              turns off all coloring (the only real purpose  of  this  is  for
              if-related definition instructions)

       noextendedregex
              turns  off  extended  regex(REG_EXTENDED) support (not likely to
              want to use this, but it's here if  needed)  {THIS  MAY  NOT  BE
              INCLUDED  UPON  COMPILE,  RUN cw -v TO SEE IF "r" IS INCLUDED IN
              THE SUPPORT}

       nopipe turns off color wrapping if the  program  is  being  piped/redi-
              rected.  (non-interactive  coloring  measure)  {THIS  MAY NOT BE
              INCLUDED UPON COMPILE, RUN cw -v TO SEE IF "t"  IS  INCLUDED  IN
              THE SUPPORT}

       nobackground
              will disable the capability to stop(^Z) the program {THIS MAY BE
              FORCED ANYWAYS, DEPENDING ON SUPPORT INCLUDED UPON COMPILE}

       noaddhelp
              will not display cw help information when --help is used in  the
              program arguments

       noeol  disables the (reset color) value placed on the end of each line

       nostrip
              turns  off  ansi  stripping of the programs output (rarely used;
              only to modify colors that already exist or have ANSI  placement
              codes)

       nowarnings
              disables  non-critical  definition  file  error messages for the
              definition instructions that follow.

       warnings
              enables non-critical definition file error messages for the def-
              inition instructions that follow.

       :[label]
              designates  the  start of a label to be used in conjunction with
              the random-goto definition instruction

       ![program]
              executes the program string as it is placed  in  the  definition
              file.   this  is  intended to either simply run a program before
              the wrapped program, or for special  purposes  relating  to  the
              ifexit definition instructions.  "{}" can be used in the program
              string to insert the command-line arguments (the program  string
              is in a shell execution format)

       @[program]
              this  is the same as !, except it does not display the output or
              allow input

       $[VAR]=[VALUE]
              sets the environmental variable VAR to VALUE {THIS  MAY  NOT  BE
              INCLUDED  UPON  COMPILE,  RUN cw -v TO SEE IF "e" IS INCLUDED IN
              THE SUPPORT}

       #[text]
              ignores the following text for processing. (for comments)

       The color values used are in the text form of:
              black, blue, green,  cyan,  red,  purple,  brown,  grey+,  grey,
              blue+,  green+,  cyan+,  red+,  purple+, yellow, white, default,
              none, random, random+, random&

       Colors with a + designate a brighter color.  random, random+  and  ran-
       dom& are random colors set at the start of cw or by the CW_RANDOM envi-
       ronmental variable or the random definition instruction (random&  is  a
       complementary color to random and random+)

ARGUMENTS
       Command-line arguments may be inserted to change some attributes of cw.
       These  arguments are not for direct use with the cw binary, they are to
       be inserted in with any other arguments passed  to  the  cw  definition
       file.  For example df --cw-nocolor -h would disable coloring and run df
       -h.

       All of the following arguments are explained in more  detail  elsewhere
       in this manual page.  The following are brief descriptions.


       +co, --cw-colorize=color[:color]
              sets the colors to the provided arguments

       +iv, --cw-invert
              inverts the internal color map

       +nc, --cw-nocolor
              disables color wrapping of the program

       +nr, --cw-norandom
              disables random colors (selects the first random choice)

       +py, --cw-usepty
              allocates  a pseudo terminal {THIS MAY NOT BE INCLUDED UPON COM-
              PILE, RUN cw -v TO SEE IF "p" IS INCLUDED IN THE SUPPORT}

       +sm, --cw-supermap=mapname
              changes the color format used (run cw -V to  see  the  supported
              supermaps)

EXAMPLES
       FILENAME /usr/lib/cw/lsof
              #!/usr/bin/cw
              path /bin:/usr/bin:/sbin:/usr/sbin:<env>
              base cyan
              digit cyan+:cyan
              match yellow:default /
              match cyan+:default -
              match cyan+:default +
              match green+:default [
              match green+:default ]
              match green+:yellow (
              match green+:default )
              match white:default *
              match blue+:default UDP
              match blue+:default TCP
              match blue+:default TPI
              ifos linux:<pseudo>
              match grey:default REG
              match grey+:default DIR
              match purple:default CHR
              match purple+:default FIFO
              match red:default DEL
              ifos-else
              match grey:default VREG
              match grey+:default VDIR
              match purple:default VCHR
              match purple+:default VFIFO
              match red:default VDEL
              ifos <any>
              match default:default COMMAND
              ifnarg -h:--help
              token green+:default 0 32

       FILENAME /usr/lib/cw/id
              #!/usr/bin/cw
              path /bin:/usr/bin:/sbin:/usr/sbin:<env>
              # shell scripts use these arguments alot, it will
              # cause problems being colored.
              ifnarg -g:-G:-n:-r:-u:--group:--groups:--name:--real:--user
              base cyan
              match cyan:default :
              match white:default -
              match white:default =
              match white:default /
              match white:cyan+ [
              match white:default ]
              match white:cyan+ (
              match white:default )
              ifnarg <none>
              nopipe

       FILENAME /usr/lib/cw/arp
              #!/usr/bin/cw
              path /sbin:/usr/sbin:/bin:/usr/bin:<env>
              random cyan:green:purple:brown
              base random
              regex white:default ([a-fA-F0-9]{1,2}:){5}+[a-fA-F0-9]{1,2}
              ifos sunos
              match random+:none Device
              match random&:none -----
              ifos-else
              match random+:none Address
              match random+:default -
              ifos <any>
              match random+:default .
              match random+:default [
              match random+:default ]
              match white:default <
              match white:default >
              match random+:default (
              match random+:default )

       FILENAME /usr/lib/cw/ifconfig
              #!/usr/bin/cw
              path /sbin:/usr/sbin:/bin:/usr/bin:<env>
              random-goto label1:label2
              :label1
              base cyan
              digit cyan+:default
              ucase cyan+:default
              match white:default :
              match white:default .
              match yellow:default (
              match yellow:default )
              match cyan+:default [
              match cyan+:default ]
              match green+:default <
              match green+:default >
              :label2
              base green
              digit green+:default
              ucase green+:default
              match yellow:default :
              match yellow:default .
              match white:default (
              match white:default )
              match green+:default [
              match green+:default ]
              match green+:default <
              match green+:default >

       FILENAME /usr/lib/cw/messages@
              #!/usr/bin/cw
              path /var/log
              base cyan
              match white:cyan+ (
              match white:default )
              match white:cyan+ [
              match white:default ]
              token white:none 0 32
              token green+:default 3 32

       FILENAME /usr/lib/cw/syslog
              #!/usr/bin/cw
              other tail -f /var/log/syslog
              base cyan
              match white:cyan+ (
              match white:default )
              match white:cyan+ [
              match white:default ]
              token white:none 0 32
              token green+:default 3 32

       FILENAME /usr/lib/cw/cw-test.cgi
              #!/usr/bin/cw
              # this is simply a mimic of the test.cgi that (tends to)
              # come with apache.  this file would go in cgi-bin.
              $SERVER_NAME=(Hidden for security purposes)
              $SERVER_ADMIN=(Hidden for security purposes)
              $SCRIPT_FILENAME=(Hidden for security purposes)
              $SERVER_SOFTWARE=(Hidden for security purposes)
              $SERVER_PORT=(Hidden for security purposes)
              $SERVER_SIGNATURE=(Hidden for security purposes)
              $SERVER_ADDR=(Hidden for security purposes)
              $DOCUMENT_ROOT=(Hidden for security purposes)
              $NOCOLOR_NEXT=1
              # change the internal color format to html.
              supermap html
              # typical cw definition instructions.
              other unset PWD;env
              random brown:green:cyan
              base random
              match random+:random& (
              match random+:default )
              match white:default =
              token random+:default 0 61
              # web browser interprets this. (color conversions with the
              # 'print' instruction need to happen after a 'supermap'
              # and/or 'random' instruction, if they are present in the
              # definition file)
              print Content-type: text/html\n\n
              print <head>\n
              print <title>[cw-test.cgi]: color test.cgi</title>\n
              print </head>\n
              print <body bgcolor="#000000" text="#ffffff">\n
              print <hr><h1>\n
              print \C[random+]cw-test.cgi\C[random&]:\C[random] env display\n
              print </h1><hr><br>\n

ENVIRONMENT
       NOCOLOR
              disables color wrapping (any value placed in the  variable  will
              enable)

       NOCOLOR_NEXT
              disables color wrapping at the next process using color wrapper.
              this is mainly useful to include in cw definition files  if  the
              program  being  wrapped is using other commands that are already
              being wrapped (this is not like stripping, it  is  for  stopping
              colors  from  being  processed  as values by programs, any value
              placed in the variable will enable)

       NOCOLOR_PIPE
              disables color wrapping if the program is being piped/redirected
              (non-interactive coloring measure, any value placed in the vari-
              able will enable) {THIS MAY NOT BE INCLUDED UPON COMPILE, RUN cw
              -v TO SEE IF "t" IS INCLUDED IN THE SUPPORT}

       CW_SHLVL
              disables  color  wrapping  if  the value this variable is set to
              does NOT equal SHLVL(set by bash/tcsh).   this  is  intended  to
              help  prevent  unwanted  coloring  outside  of the current users
              shell (usually should be set to 1)

       CW_CHK_NOCOLOR
              disables color wrapping if the program/shell  expression  placed
              in  the variable returns a non-zero exit code.  this is intended
              to help prevent unwanted coloring on  a  system-specific  basis.
              "{}"  can  be  used  in  the variable to insert the command-line
              arguments {THIS MAY NOT BE INCLUDED UPON COMPILE, RUN cw  -v  TO
              SEE IF "w" IS INCLUDED IN THE SUPPORT}

       CW_CHK_SETCODE
              sets  the  internal  exit  code to the exit code returned by the
              program/shell expression placed in the variable to be used  with
              the  ifexit  definition instructions, provided no ! or @ defini-
              tion instructions are called before.  "{}" can be  used  in  the
              variable  to  insert the command-line arguments {THIS MAY NOT BE
              INCLUDED UPON COMPILE, RUN cw -v TO SEE IF "w"  IS  INCLUDED  IN
              THE SUPPORT}

       CW_USEPTY
              allocates a pty to emulate a programs natural environment.  this
              can be useful for programs that expect a terminal  to  be  used,
              this  may  include  programs  that  appear to hang and dump text
              either when the program exits or in large chunks while  running.
              (any  value placed in the variable will enable) {THIS MAY NOT BE
              INCLUDED UPON COMPILE, RUN cw -v TO SEE IF "p"  IS  INCLUDED  IN
              THE SUPPORT}

       CW_RANDOM
              creates  a  new  random colorset based on a list of colors sepa-
              rated by colons.  the random color selected will be used for the
              colors random, random+ and random& in definition instructions to
              come (color values explicitly used in context to the random def-
              inition  instruction are: black, blue, green, cyan, red, purple,
              brown, grey, grey+)

       CW_NORANDOM
              disables random colors by always using  the  first  list  choice
              with  the  random  and  random-goto definition instructions (any
              value placed in the variable will enable)

       CW_INVERT
              re-defines the internal colormap to the opposite  colors.   this
              is intended to help terminals with white backgrounds become more
              readable (any value placed in the variable will enable)

       CW_COLORIZE
              defines a static colorset to override  the  definition  file(and
              CW_INVERT)  colors.   this  is  intended  to help make a uniform
              color scheme.  the format is CW_COLORIZE=color[:color] ('['  and
              ']'  are  not  included).  if a second color is provided you may
              use any colors desired for both fields,  however  if  you  place
              just  one  color in the variable it must be one of the following
              colors: black, blue, green,  cyan,  red,  purple,  brown,  grey,
              grey+, random (using the dual color entry style can cause irreg-
              ular coloring using offbeat combinations do to the method  being
              used  to  colorize,  it  is  recommended to use the single entry
              style)

       CW_REMAP
              remaps one or more internal color(s) to  ANSI  values  or  other
              internal  color  values.  this is useful for remapping colors to
              special ANSI (code) values or more advanced color scheming (than
              CW_COLORIZE).   the colors random, random+, random&, default and
              none  may  not   be   remapped   or   used.    the   format   is
              CW_REMAP=color=[##;##|color]:color=[##;##|color]:... ('#' stands
              for a single digit, which forms the ANSI code;  '|'  stands  for
              "or", and the '|' is not included; '[' and ']' are not included)

       CW_SUPERMAP
              changes the internal color format to an entirely different color
              format(ie.  non-ANSI),  this is intended for special non-console
              coloring situations.  supermaps are internal and can  be  listed
              by  running  cw -V.  if a supermap and a remap are both defined,
              then the supermap will be forcefully disabled due  to  potential
              internal conflicts

       CW_HEADER
              defines the path to a text file to display before the program is
              launched (the header definition instruction overrides this)

       CW_FOOTER
              defines the path to a text file to display after the program has
              completed (the footer definition instruction overrides this)

       CW_LBORDER
              prepends  the  data in the variable to each (colored) line, this
              is intended for special processing outside of cw.  the  data  in
              the variable is not colored

       CW_RBORDER
              appends the data in the variable to each (colored) line, this is
              intended for special processing outside of cw.  the data in  the
              variable is not colored

       CW_CLEAR
              clears the screen before any program (wrapped by cw) starts (any
              value placed in the variable will enable)

AUTHOR
       Written by v9/fakehalo. [v9@fakehalo.us]

BUGS
       Report bugs to <v9@fakehalo.us>.

COPYRIGHT
       Copyright (C) 2004 v9/fakehalo.
       This is free software; see the source for copying conditions.  There is
       NO  warranty;  not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR
       PURPOSE.

SEE ALSO
       cwu(1) cwe(1)
