FAQ-style readme:

1. What is cw(color wrapper)?
2. How does cw work?
3. How do I install cw?
4. How do I enable use of cw?
5. How do I turn cw on and off?
6. Is there a more system-specific way to turn coloring on and off?
7. How can I make/modify the coloring of a specific program?
8. Can ncurses programs be color wrapped? 
9. There are too many different colors, how do I get static colors?
10. What are headers and footers, and how do I use them? 
11. My terminal has a white background, why can't I see text?
12. The program I'm running doesn't look/run like it normally does, why?
13. I get "bad interpreter" errors when I run definition files, why? 
14. Is cw only intended to color in ANSI?
15. What is "cwe" used for?



1. What is cw(color wrapper)?

cw is a non-intrusive real-time ANSI color wrapper for common unix-based
commands on GNU/linux.  cw is designed to simulate the environment of the
commands being executed, so that if a person types 'du', 'df', 'ping', etc.
in their shell it will automatically color the output in real-time according
to a definition file containing the color format desired.  cw has support for
wildcard match coloring, tokenized coloring, headers/footers, case scenario
coloring, command line dependent definition coloring, and includes over 50
pre-made definition files.


2. How does cw work?

cw works by having an augmented PATH environmental variable which has a
path to the cw definition directory prepended to it, this is usually set
as "export PATH=/usr/local/lib/cw:$PATH" (in bash).  in this cw definition
directory you will see file(s) under the very same name of the program
they are designed to color.  these files appear as the common executable
binaries you may be used to using, but they are just text files that
contain instructions for cw to use, including the path to the "real"
binary.


3. How do I install cw?

see the ./INSTALL text file.


4. How do I enable use of cw?

if cw has been installed, a shell script named "colorcfg"(or 
~/.cw/bin/colorcfg with a local install) was also installed, the syntax
is [1|2|3].  the higher the number the more precautions have been set to
stop "unwanted" coloring.  1 is the prepending of the PATH environmental
variable to your bash/tcsh rc scripts(no precautions).  2 will stop
coloring if the program has been piped/redirected(NOCOLOR_PIPE=1).  3 will
stop coloring if the program has been piped/redirected or the shell is not
in the proper shell level(SHLVL set by bash/tcsh, CW_SHLVL=$SHLVL).  usually
1 is fine, but if unwanted coloring occurs try the higher numbers.  if the
"colorcfg" script is not available these can be set manually(follow the
instructions after the install or view the "cw" manpage(or./doc/README.cw)).


5. How do I turn cw on and off?

(see question 4 if you haven't read it already, to get to this point)
if cw has been installed, a shell script named "color"(or ~/.cw/bin/color
with a local install) was also installed, the syntax is [on|off] or toggled
if no arguments are supplied.  if the "color" script is not available you
may set NOCOLOR=1 in your environment, or create(touch) ~/.nocolor.


6. Is there a more system-specific way to turn coloring on and off?

if all the anti-coloring precautions implemented into cw are not
enough, you may make your own program/shell expression to test the 
system environment.  this is done by setting CW_CHK_NOCOLOR="program/
shell expression" in the environment, and if the exit code returned is
non-zero it will disable the coloring.  for example, setting
CW_CHK_NOCOLOR='if [ "`whoami`" = "root" ];then exit 1;fi' would disable
coloring for the root user only.


7. How can I make/modify the coloring of a specific program?

first off I recommend viewing the "cw" manpage(or ./doc/README.cw) for
a little understanding on the instructions available to use.  after that
it's as simple as opening/creating the selected definition file (ex.
/usr/local/lib/cw/df) in your favorite text editor and making the
desired changes.


8. Can ncurses programs be color wrapped? 

it depends.  for example the 'top' program can be color wrapped(with
the use of the "nostrip" definition instruction), but for most ncurses
programs it is too unstable to properly color wrap.  generally speaking,
it is on a program-to-program basis. (the "nostrip" definition instruction
will almost always need to be used in relation to wrapping ncurses programs,
and in some cases the "usepty" definition instruction will need to be used)


9. There are too many different colors, how do I get static colors?

if you find that there are too many color schemes for each program
and/or the randomness of the default definition files is too much for
you, then you may want to set CW_COLORIZE in your environment.  see the
"cw" manual page(or ./doc/README.cw) on the proper syntax of CW_COLORIZE.
(for more advanced color scheming see the CW_REMAP environmental variable,
also referenced in the manual page)


10. What are headers and footers, and how do I use them? 

headers and footers are (paths to) text files placed before the program
begins and after it has completed.  these files are typically going to
be ANSI art.  the headers and footers can be defined via definition
instructions inside a definition file(see the "cw" manual page or
./doc/README.cw), or by being placed in the environment under CW_HEADER
and CW_FOOTER.  definition instructions override the environment.  there
are generic headers and footers that come with the cw package in the
./etc directory(installed into the definition files directory).  headers
and footers can take up alot of screen room, setting CW_CLEAR=1
in the environment(to clear the screen before the program starts) may 
help make a better appearance.


11. My terminal has a white background, why can't I see text?

cw is designed, by default, to expect a black background.  however, 
you may set CW_INVERT=1 in your environment to invert cw's internal
colorset.  this will make black become white and the lighter colors
become the darker colors.


12. The program I'm running doesn't look/run like it normally does, why?

depending on the program you are running it may expect to be executed in
a terminal.  symptoms of this may appear as text dumping in large chunks
while the program is running, text dumping when the program exits, or a
completely different layout of the text output.  to help resolve the
issue you may emulate a pty terminal by setting CW_USEPTY=1 in your
environment, or by using the "usepty" definition instruction in your
definition file.


13. I get "bad interpreter" errors when I run definition files, why? 

this usually means the "cw" binary has moved or never been set to the
definition file(s).  see the "cwu" manpage(or ./doc/README.cw) to update
the definition file directory.  generally this is as simple as typing
"cwu /path/to/new/def/dir `which cw`" in your shell.


14. Is cw only intended to color in ANSI?

yes and no, the main intention of cw is to color in ANSI.  however,
additions have been made to color in other formats via "supermapping". 
these supermaps can be set via the definition file using the "supermap"
definition instruction, or the environment using CW_SUPERMAP.  all
supermaps are internal and can be listed by running "cw -V".


15. What is "cwe" used for?

cwe interprets a single string passed via the command-line and
outputs a formated version to standard output, similar to that of
"echo -e".  this is useful for special purposes such as coloring
shell prompts, use in shell scripts, and other misc. things while
still keeping with (some of) the color scheme of cw.  for more
information see the "cwe" manpage(or./doc/README.cwe).
