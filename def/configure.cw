# this is a hack of sorts.  configure scripts may include some third party
# programs that may be colored by cw, if so they could fail do to the
# colors(ANSI) included.  this will simply turn the color off during the
# next process.  same as make's, sometimes can be the same thing.
$NOCOLOR_NEXT=1
other ./configure
base grey+
match green+:default yes$
match red+:default no$
match cyan:default \.
match grey:default -
match grey:default /
match cyan:default \\
match cyan+:default :
match cyan+:default '
match cyan+:default `
match cyan+:default "
match green+:default &
match white:default =
match white:default \[
match white:default \]
match green+:default \*\*\*
match white:default \*
match white:default checking
match yellow:default updating
match cyan+:default creating
match cyan+:default loading
match grey:default (cached)
match green+:default warning: 
match red+:default error: 
match red+:default Error
match yellow:default Interrupt
