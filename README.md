# cw

(c) 2013-2024 Reuben Thomas <rrt@sc3d.org>  
(c) 2004-2010 v9/fakehalo <v9@fakehalo.us>  

https://github.com/rrthomas/cw/  

cw colors the output of command-line programs.  It is designed to be
transparent: when you type `du`, `df`, `ping`, etc. in your shell the output
is automatically colored according to a definition script, but when the
commands are used non-interactively (e.g. in another script) their output is
not colored, so as not to confuse programs that process their output.  Text
patterns are defined as lexical classes, and assigned colors.  The arguments
to the original program and OS under which it is running can be tested.  cw
includes over 50 definition scripts, and it’s easy to write more.

The colors to use are defined by definition scripts, which have the same
name as the program they color.

cw should not cause any security problems, but since wrapping processes and
changing their output can result in subtle bugs, it is not recommended for
use by privileged users such as root.

cw is distributed under the GNU General Public License version 3, or, at
your option, any later version.  This is free software: you are free to
change and redistribute it.  There is NO warranty, to the extent permitted
by law.

Please report bugs in the issue tracker (see above for the address).


## Installation

Install cw using LuaRocks from https://www.luarocks.org/ :

```
luarocks install cw
```

To use cw, you need to prepend its definition directory to the PATH
environment variable.  You should do this in your shell’s interactive
startup file (you don’t want cw being run by background commands).
For example, for bash and other Bourne-compatible shells, add the
following line to your `~/.bashrc`:

```
if [ -n "$PS1" ]; then PATH=`cw-definitions-path`:$PATH; export PATH; fi
```

The test of `PS1` ensures that the shell is really interactive.


## Use

Start a new shell and try some commands, which should now be colored!

The `cw` command itself has `--version` and `--help`, and explains that it’s not meant to be run directly.


## Alternatives to cw

cw doesn’t try to replace some dedicated coloring wrappers and
programs, such as:

ccal: http://unicorn.us.com/cal.html  
colorgcc: https://schlueters.de/colorgcc.html  
colordiff: https://www.colordiff.org/  
colormake: https://github.com/pagekite/Colormake  
freecolor: https://www.rkeene.org/oss/freecolor/  

There are also other programs like cw that you may prefer:

Crayonizer: https://sites.google.com/site/columscode/home/crayonizer  
acoc: http://www.caliban.org/ruby/acoc.shtml  


## Definition scripts

Definition scripts are written in [the Lua scripting language][1] but you don’t need to know Lua to make a simple script.

The instructions that follow assume you have checked the cw project out from GitHub (see top for URL).

To make your own, try first modifying an existing script; remember to make your script executable, and please consider submitting it to GitHub (see top) for inclusion in future releases.

[1]: https://www.lua.org

Definition scripts, which have the same name as the program they color, are
in the ‘script’ subdirectory.

cw provides scripts with a standard Lua environment, plus `luaposix` (useful
for tasks such as checking the system type and setting environment
variables) and the following extra features:

* `match(class, pattern)` This function adds a rule to color text matching
  `pattern` as class `class`. Matches are applied in order of definition.
  See “Classes and colors” below for an explanation of classes, and
  [the Lua manual][2] for the syntax of patterns.

[2]: https://www.lua.org/manual/5.1/manual.html#5.4.1

* `arg` A list of command-line arguments to the program being colored.  For
  convenience, each argument is also a key in the table, so it is easy to
  test whether a certain argument has been given, in any position, with code
  such as `if arg["--help"] then…`
  
## Classes and colors

The default classes are: `highlight, lowlight, warning, error, default`.
The names suggest the sort of text patterns you may want to map to them.
The mapping of classes to colors can be changed with the `CW_COLORS` environment variable (see “Environment variables”, below).

The available colors are: `black, red, green, yellow, blue, magenta, cyan,
white`.  A color may be prefixed `bright` to denote a brighter color,
e.g. `bright red` (note the space).


## Environment variables

* `NOCOLOR` Disables color wrapping when set.
  Note that cw disables coloring if standard output is not connected to a terminal, or if the terminal type `TERM` is `dumb`.
* `CW_INVERT` Inverts the built-in color scheme when set, to make cw more legible on terminals with white backgrounds (cw is set up for a black background by default).
* `CW_COLORS` Defines a mapping from classes to colors.  The format is
  `CW_COLORS=class=color[:class=color...]`.


## Recommendations for definition scripts

There are two obvious reasons to color commands: first, for aesthetic
reasons, and secondly, to make the output more legible.  For the first kind,
use your imagination!  The colorers provided with cw are of the second kind,
and the following suggestions aim to help improve presentation:

* Colors should be used to pick out key information; it is not necessary to color everything.
* Many commands’ output is already clear, and needs little or no coloring.
  For example, color just the column headings of tables.
* When each line is in the same format, but unaligned, color can be helpful
  to pick out particular fields; for example, the coloring of variable names
  in the `env` colorer.
* Since `cw` is not recommended for use by privileged users, don’t bother point writing wrappers for commands that typically require administrator privileges to use.
* Help output (e.g. the output of `--help`) should not be colored.
* Use of the `default` class only makes sense to override the coloring of text that has already been colored by another pattern.
